
#include "iqm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

/////////////////////////////////////////////////////////////////////
#include "common.h"
#include "IqmModel.h"
#include "engine/Timer.h"
#include "graphic/ShaderProgram.h"

using namespace mt::engine;
using namespace mt::graphic;

ShaderProgram IqmModel::shader;

/////////////////////////////////////////////////////////////////////


#ifndef GL_GENERATE_MIPMAP
#define GL_GENERATE_MIPMAP 0x8191
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

/************************************************************************************
 * Some defines
 */

#define WIN_PROG_VERSION  "IqeBrowser / V1.10 2014-05-02"

#define WIN_DEFAULT_TITLE  "IQE model browser and compiler"

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define CLAMP(x,a,b) MIN(MAX(x,a),b)

/************************************************************************************
 * Some vector and matrix math.
 */

typedef float vec3[3];
typedef float vec4[4];
typedef float mat4[16];

struct pose {
	vec3 location;
	vec4 rotation;
	vec3 scale;
};

#define A(row,col) a[(col<<2)+row]
#define B(row,col) b[(col<<2)+row]
#define M(row,col) m[(col<<2)+row]

static void mat_copy(mat4 p, const mat4 m)
{
	memcpy(p, m, sizeof(mat4));
}

static void mat_mul(mat4 m, const mat4 a, const mat4 b)
{
	int i;
	for (i = 0; i < 4; i++) {
		const float ai0=A(i,0), ai1=A(i,1), ai2=A(i,2), ai3=A(i,3);
		M(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
		M(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
		M(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
		M(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
	}
}

static void mat_invert(mat4 out, const mat4 m)
{
	mat4 inv;
	float det;
	int i;

	inv[0] = m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15] +
		m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
	inv[4] = -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15] -
		m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
	inv[8] = m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15] +
		m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
	inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14] -
		m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
	inv[1] = -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15] -
		m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
	inv[5] = m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15] +
		m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
	inv[9] = -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15] -
		m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
	inv[13] = m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14] +
		m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
	inv[2] = m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15] +
		m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
	inv[6] = -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15] -
		m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
	inv[10] = m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15] +
		m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
	inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14] -
		m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
	inv[3] = -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11] -
		m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
	inv[7] = m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11] +
		m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
	inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11] -
		m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
	inv[15] = m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10] +
		m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	assert (det != 0);
	det = 1.0 / det;
	for (i = 0; i < 16; i++)
		out[i] = inv[i] * det;
}

static void mat_from_pose(mat4 m, const vec3 t, const vec4 q, const vec3 s)
{
	float x2 = q[0] + q[0];
	float y2 = q[1] + q[1];
	float z2 = q[2] + q[2];
	{
		float xx2 = q[0] * x2;
		float yy2 = q[1] * y2;
		float zz2 = q[2] * z2;
		M(0,0) = 1 - yy2 - zz2;
		M(1,1) = 1 - xx2 - zz2;
		M(2,2) = 1 - xx2 - yy2;
	}
	{
		float yz2 = q[1] * z2;
		float wx2 = q[3] * x2;
		M(2,1) = yz2 + wx2;
		M(1,2) = yz2 - wx2;
	}
	{
		float xy2 = q[0] * y2;
		float wz2 = q[3] * z2;
		M(1,0) = xy2 + wz2;
		M(0,1) = xy2 - wz2;
	}
	{
		float xz2 = q[0] * z2;
		float wy2 = q[3] * y2;
		M(0,2) = xz2 + wy2;
		M(2,0) = xz2 - wy2;
	}

	m[0] *= s[0]; m[4] *= s[1]; m[8] *= s[2];
	m[1] *= s[0]; m[5] *= s[1]; m[9] *= s[2];
	m[2] *= s[0]; m[6] *= s[1]; m[10] *= s[2];

	M(0,3) = t[0];
	M(1,3) = t[1];
	M(2,3) = t[2];

	M(3,0) = 0;
	M(3,1) = 0;
	M(3,2) = 0;
	M(3,3) = 1;
}

#undef A
#undef B
#undef M

static float vec_dist2(const vec3 a, const vec3 b)
{
	float d0, d1, d2;
	d0 = a[0] - b[0];
	d1 = a[1] - b[1];
	d2 = a[2] - b[2];
	return d0 * d0 + d1 * d1 + d2 * d2;
}

static void vec_scale(vec3 p, const vec3 v, float s)
{
	p[0] = v[0] * s;
	p[1] = v[1] * s;
	p[2] = v[2] * s;
}

static void vec_add(vec3 p, const vec3 a, const vec3 b)
{
	p[0] = a[0] + b[0];
	p[1] = a[1] + b[1];
	p[2] = a[2] + b[2];
}

static void vec_copy(vec3 p, const vec3 a)
{
	p[0] = a[0];
	p[1] = a[1];
	p[2] = a[2];
}

static void vec_minmax( vec3 min, vec3 max, const vec3 a)
{
	if( min[0] > a[0]) min[0] = a[0];
	if( min[1] > a[1]) min[1] = a[1];
	if( min[2] > a[2]) min[2] = a[2];

	if( max[0] < a[0]) max[0] = a[0];
	if( max[1] < a[1]) max[1] = a[1];
	if( max[2] < a[2]) max[2] = a[2];

}

static void mat_vec_mul(vec3 p, const mat4 m, const vec3 v)
{
	assert(p != v);
	p[0] = m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12];
	p[1] = m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13];
	p[2] = m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14];
}

static void mat_vec_mul_n(vec3 p, const mat4 m, const vec3 v)
{
	assert(p != v);
	p[0] = m[0] * v[0] + m[4] * v[1] + m[8] * v[2];
	p[1] = m[1] * v[0] + m[5] * v[1] + m[9] * v[2];
	p[2] = m[2] * v[0] + m[6] * v[1] + m[10] * v[2];
}

static void calc_mul_matrix(mat4 *skin_matrix, mat4 *abs_pose_matrix, mat4 *inv_bind_matrix, int count)
{
	int i;
	for (i = 0; i < count; i++)
		mat_mul(skin_matrix[i], abs_pose_matrix[i], inv_bind_matrix[i]);
}

static void calc_inv_matrix(mat4 *inv_bind_matrix, mat4 *abs_bind_matrix, int count)
{
	int i;
	for (i = 0; i < count; i++)
		mat_invert(inv_bind_matrix[i], abs_bind_matrix[i]);
}

static void calc_abs_matrix(mat4 *abs_pose_matrix, mat4 *pose_matrix, int *parent, int count)
{
	int i;
	for (i = 0; i < count; i++)
		if (parent[i] >= 0)
			mat_mul(abs_pose_matrix[i], abs_pose_matrix[parent[i]], pose_matrix[i]);
		else
			mat_copy(abs_pose_matrix[i], pose_matrix[i]);
}

static void calc_matrix_from_pose(mat4 *pose_matrix, struct pose *pose, int count)
{
	int i;
	for (i = 0; i < count; i++)
		mat_from_pose(pose_matrix[i], pose[i].location, pose[i].rotation, pose[i].scale);
}

#define Vector4Inverse( v )			( ( v )[0] = -( v )[0], ( v )[1] = -( v )[1], ( v )[2] = -( v )[2], ( v )[3] = -( v )[3] )

float Vector4Normalize( float v[] )
{
	float length, ilength;

	length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3];

	if( length )
	{
		length = sqrt( length ); // FIXME
		ilength = 1.0/length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
		v[3] *= ilength;
	}

	return length;
}

/*
 * Use Sean Barrett's excellent stb_image to load textures.
 */

// #define STBI_NO_HDR
#include "stb/stb_image.h"

// ----------------------------------

static unsigned char checker_data[256*256];
static unsigned int checker_texture = 0;

static void initchecker(void)
{
	int x, y, i = 0;
	for (y = 0; y < 256; y++) {
		for (x = 0; x < 256; x++) {
			int k = ((x>>5) & 1) ^ ((y>>5) & 1);
			checker_data[i++] = k ? 255 : 192;
		}
	}
	// glGenTextures(1, &checker_texture);
	// glBindTexture(GL_TEXTURE_2D, checker_texture);
	// glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, checker_data);
}

static void lowerstring(char *s)
{
	while (*s) { *s = tolower(*s); s++; }
}

static unsigned int loadtexture(char *filename)
{
	unsigned int texture;
	unsigned char *image;
	int w, h, n, intfmt = 0, fmt = 0;

	image = stbi_load(filename, &w, &h, &n, 0);
	if (!image) {
		lowerstring(filename);
		image = stbi_load(filename, &w, &h, &n, 0);
		if (!image) {
			fprintf(stderr, "cannot load texture '%s'\n", filename);
			return 0;
		}
	}

	// if (n == 1) { intfmt = fmt = GL_LUMINANCE; }
	// if (n == 2) { intfmt = fmt = GL_LUMINANCE_ALPHA; }
	// if (n == 3) { intfmt = fmt = GL_RGB; }
	// if (n == 4) { intfmt = fmt = GL_RGBA; }

	// glGenTextures(1, &texture);
	// glBindTexture(GL_TEXTURE_2D, texture);
	// glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// glTexImage2D(GL_TEXTURE_2D, 0, intfmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, image);
	// //glGenerateMipmap(GL_TEXTURE_2D);

	free(image);

	return texture;
}


static char *FileTextureFileTypesTable[] = { (char*)"png", (char*)"jpg", (char*)"tga" };
static int   FileTextureFileTypesN = sizeof( FileTextureFileTypesTable) / sizeof( char *);

static int loadmaterial(char *material, char *basedir)
{
	int texture, iFiltType;
	char filename[2000], *pMaterialName;

	// get pointer to material name

	pMaterialName = strrchr(material, ';');
	if (pMaterialName) pMaterialName = pMaterialName + 1;
	else pMaterialName = material;

	// test this directory
  for( iFiltType = 0; iFiltType < FileTextureFileTypesN; iFiltType++) {

	  sprintf(filename, "%s/%s.%s", basedir, pMaterialName, FileTextureFileTypesTable[ iFiltType]);
	  texture = loadtexture(filename);
	  if( texture) {
      goto ExitPoint;
	  }
  }

	// test textures subdirectory
  for( iFiltType = 0; iFiltType < FileTextureFileTypesN; iFiltType++) {

		sprintf(filename, "%s/textures/%s.%s", basedir, pMaterialName, FileTextureFileTypesTable[ iFiltType]);
	  texture = loadtexture(filename);
	  if( texture) {
      goto ExitPoint;
	  }
  }

	// test 'skin' texture in this directory
  for( iFiltType = 0; iFiltType < FileTextureFileTypesN; iFiltType++) {

		sprintf(filename, "%s/skin.%s", basedir, FileTextureFileTypesTable[ iFiltType]);
	  texture = loadtexture(filename);
	  if( texture) {
      goto ExitPoint;
	  }
  }

ExitPoint:

	// if (texture) {
	// 	if (strstr(material, "clamp;")) {
	// 		glBindTexture(GL_TEXTURE_2D, texture);
	// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// 	}
	// 	return texture;
	// }

	return checker_texture;
}

/*
 * IQE loading and drawing
 */

#define IQE_MAGIC "# Inter-Quake Export"
#define MAXBONE 256

struct model {
	struct skel *skel;             // skeleton
	struct mesh *mesh;             // mesh

	// anim data
	int anim_poses;                // number of poses in an animation frame
	int anim_count;                // number of animations
	struct anim *anim;             // list of animations
};

struct skel {
	int joint_count;               // number of joints
	int parent[MAXBONE];
	char *name[MAXBONE];
	struct pose pose[MAXBONE];
	int  TempDrawPosX[MAXBONE];    // temp use for draing bone names in 3D screen
	int  TempDrawPosY[MAXBONE];    // temp use for draing bone names in 3D screen
};

struct mesh {
	int vertex_count;
	float *position, *normal, *texcoord, *color;
	float *blendweight;
	int *blendindex;

	int element_count;
	int *element;

	int part_count;
	struct part *part;

	float *aposition, *anormal;

	mat4 abs_bind_matrix[MAXBONE];
	mat4 inv_bind_matrix[MAXBONE];
};

struct part {
  char *pMaterialString;     // material string
  char *pMaterialBasedir;    // directory to use for material lookup
	int material;              // openGL material index, -1 if no index assigned
	int first, count;          // triangles for the this partial mesh
};

struct anim {
	char *name;
	int len, cap;
	float framerate;
	struct pose **data;
	struct anim *prev, *next;
};

struct floatarray {
	int len, cap;
	float *data;
};

struct intarray {
	int len, cap;
	int *data;
};

struct partarray {
	int len, cap;
	struct part *data;
};

/************************************************************************************
 * Models to draw on GUI
 */

// current model loaded from file selection

struct model *pFileSelModel;       // file selected model
char   FileSelName[ 1024];         // file selected model, file name
char   FileSelFullName[ 1024];     // file selected model, path and file name

// model container

#define MODEL_CONTAINER_DISP 16    // model container, max number of models to display
#define MODEL_CONTAINER_MAX  64    // max number of models in model container

typedef struct {
  char   ModelName[ 1024];       // file selected model, file name
  char   ModelFullName[ 1024];   // file selected model, path and file name
  int    part_count_before_add;  // number of meshes before any add of mesh or skeleton
  model *pModel;                 // parsed model data
} T_ModelContainer;

T_ModelContainer ModelContainer[ MODEL_CONTAINER_MAX]; // the model container
int    ModelContainer_n = 0;                           // number of models in container

// draw this model
static struct model *pDrawModel = NULL;   // drawing this model

static int curframe = 0;
static struct anim *curanim = NULL;
static float curtime = 0;
static int lasttime = 0;

// or draw this message

static float DrawMessageColR, DrawMessageColG, DrawMessageColB;
static char DrawMessageText1[ 1024];
static char DrawMessageText2[ 1024];
static char DrawMessageText3[ 1024];

/*
============================================================================

					BYTE ORDER FUNCTIONS

============================================================================
*/

static int	bigendien;

// can't just use function pointers, or dll linkage can
// mess up when qcommon is included in multiple places
static short	(*_BigShort) (short l);
static short	(*_LittleShort) (short l);
static int		(*_BigLong) (int l);
static int		(*_LittleLong) (int l);
static float	(*_BigFloat) (float l);
static float	(*_LittleFloat) (float l);

static short	BigShort(short l){return _BigShort(l);}
static short	LittleShort(short l) {return _LittleShort(l);}
static int		BigLong (int l) {return _BigLong(l);}
static int		LittleLong (int l) {return _LittleLong(l);}
static float	BigFloat (float l) {return _BigFloat(l);}
static float	LittleFloat (float l) {return _LittleFloat(l);}

static short   ShortSwap (short l)
{
	unsigned char    b1,b2;

	b1 = l&255;
	b2 = (l>>8)&255;

	return (b1<<8) + b2;
}

static short	ShortNoSwap (short l)
{
	return l;
}

static int    LongSwap (int l)
{
	unsigned char    b1,b2,b3,b4;

	b1 = l&255;
	b2 = (l>>8)&255;
	b3 = (l>>16)&255;
	b4 = (l>>24)&255;

	return ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4;
}

static int	LongNoSwap (int l)
{
	return l;
}

static float FloatSwap (float f)
{
	union
	{
		float	f;
		unsigned char	b[4];
	} dat1, dat2;


	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}

static float FloatNoSwap (float f)
{
	return f;
}

/*
================
Swap_Init
================
*/
static void Swap_Init (void)
{
	unsigned char	swaptest[2] = {1,0};

// set the byte swapping variables in a portable manner
	if ( *(short *)swaptest == 1)
	{
		bigendien = false;
		_BigShort = ShortSwap;
		_LittleShort = ShortNoSwap;
		_BigLong = LongSwap;
		_LittleLong = LongNoSwap;
		_BigFloat = FloatSwap;
		_LittleFloat = FloatNoSwap;
	}
	else
	{
		bigendien = true;
		_BigShort = ShortNoSwap;
		_LittleShort = ShortSwap;
		_BigLong = LongNoSwap;
		_LittleLong = LongSwap;
		_BigFloat = FloatNoSwap;
		_LittleFloat = FloatSwap;
	}
}

/************************************************************************************
 * loadmodel() helper functions
 */

/* global scratch buffers */
static struct floatarray position = { 0, 0, NULL };
static struct floatarray normal = { 0, 0, NULL };
static struct floatarray texcoord = { 0, 0, NULL };
static struct floatarray color = { 0, 0, NULL };
static struct intarray blendindex = { 0, 0, NULL };
static struct floatarray blendweight = { 0, 0, NULL };
static struct intarray element = { 0, 0, NULL };
static struct partarray partbuf = { 0, 0, NULL };

static void *duparray(void *data, int count, int size)
{
	if (count == 0)
		return NULL;

	if (data == NULL)
		return NULL;

	void *p = malloc(count * size);
	memcpy(p, data, count * size);
	return p;
}

static inline void pushfloat(struct floatarray *a, float v)
{
	if (a->len + 1 >= a->cap) {
		a->cap = 600 + a->cap * 2;
		a->data = (float *)realloc(a->data, a->cap * sizeof(*a->data));
	}
	a->data[a->len++] = v;
}

static inline void pushint(struct intarray *a, int v)
{
	if (a->len + 1 >= a->cap) {
		a->cap = 600 + a->cap * 2;
		a->data = (int *)realloc(a->data, a->cap * sizeof(*a->data));
	}
	a->data[a->len++] = v;
}

static void pushpart( struct partarray *a, int first, int last, int material,
                      char *pMaterialString, char *pMaterialBasedir)
{
	/* merge parts if they share materials */
	if (a->len > 0 && (int)a->data[a->len-1].material == material) {
		a->data[a->len-1].count += last - first;
		return;
	}
	if (a->len + 1 >= a->cap) {
		a->cap = 600 + a->cap * 2;
		a->data = (part *)realloc(a->data, a->cap * sizeof(*a->data));
	}
	a->data[a->len].first = first;
	a->data[a->len].count = last - first;
	a->data[a->len].material = material;
	a->data[a->len].pMaterialString  = strdup( pMaterialString != NULL ? pMaterialString : "");
	a->data[a->len].pMaterialBasedir = strdup( pMaterialBasedir);
	a->len++;
}

static struct anim *pushanim(struct anim *head, char *name)
{
	struct anim *anim = (struct anim *)malloc(sizeof(struct anim));
	anim->name = strdup(name);
	anim->len = anim->cap = 0;
	anim->data = NULL;
	anim->framerate = 10.0;          // preset 10 fps
	if (head) head->prev = anim;
	anim->next = head;
	anim->prev = NULL;
	return anim;
}

static struct pose *pushframe(struct anim *a, int bone_count)
{
	struct pose *pose = (struct pose *)malloc(sizeof(struct pose) * bone_count);;
	if (a->len + 1 >= a->cap) {
		a->cap = 128 + a->cap * 2;
		a->data = (struct pose **)realloc(a->data, a->cap * sizeof(*a->data));
	}
	a->data[a->len++] = pose;
	return pose;
}

static void addposition(float x, float y, float z)
{
	pushfloat(&position, x);
	pushfloat(&position, y);
	pushfloat(&position, z);
}

static void addnormal(float x, float y, float z)
{
	pushfloat(&normal, x);
	pushfloat(&normal, y);
	pushfloat(&normal, z);
}

static void addtexcoord(float u, float v)
{
	pushfloat(&texcoord, u);
	pushfloat(&texcoord, v);
}

static void addcolor(float x, float y, float z, float w)
{
	pushfloat(&color, x);
	pushfloat(&color, y);
	pushfloat(&color, z);
	pushfloat(&color, w);
}

static void addblend(int a, int b, int c, int d, float x, float y, float z, float w)
{
	float total = x + y + z + w;
	pushint(&blendindex, a);
	pushint(&blendindex, b);
	pushint(&blendindex, c);
	pushint(&blendindex, d);
	pushfloat(&blendweight, x / total);
	pushfloat(&blendweight, y / total);
	pushfloat(&blendweight, z / total);
	pushfloat(&blendweight, w / total);
}

static void addtriangleFlipWinding(int a, int b, int c)
{
	// flip triangle winding
	pushint(&element, c);
	pushint(&element, b);
	pushint(&element, a);
}

static char *parsestring(char **stringp)
{
	char *start, *end, *s = *stringp;
	while (isspace(*s)) s++;
	if (*s == '"') {
		s++;
		start = end = s;
		while (*end && *end != '"') end++;
		if (*end) *end++ = 0;
	} else {
		start = end = s;
		while (*end && !isspace(*end)) end++;
		if (*end) *end++ = 0;
	}
	*stringp = end;
	return start;
}

static char *parseword(char **stringp)
{
	char *start, *end, *s = *stringp;
	while (isspace(*s)) s++;
	start = end = s;
	while (*end && !isspace(*end)) end++;
	if (*end) *end++ = 0;
	*stringp = end;
	return start;
}

static inline float parsefloat(char **stringp, float def)
{
	char *s = parseword(stringp);
	return *s ? atof(s) : def;
}

static inline int parseint(char **stringp, int def)
{
	char *s = parseword(stringp);
	return *s ? atoi(s) : def;
}

/************************************************************************************
 * IQE_loadmodel()
 */

static struct model *IQE_loadmodel(char *filename)
{
	static mat4 loc_bind_matrix[MAXBONE];

  char basedir[ 1024];
	FILE *fp;
	char line[256];
	int material = -1;           // preset no material loaded
	char MaterialString[ 1024] = "";
	int first = 0;
	int fm = 0;
	char *s, *sp, *p;

	fprintf( stderr, "loading iqe model '%s' ...\n", filename);

	position.len = 0;
	normal.len = 0;
	texcoord.len = 0;
	color.len = 0;
	blendindex.len = 0;
	blendweight.len = 0;
	element.len = 0;
  partbuf.len = 0;

  // get base directory (path to filename)

	strcpy(basedir, filename);
	p = strrchr(basedir, '/');
	if (!p) p = strrchr(basedir, '\\');
	if (!p) {
    strcpy(basedir, "");    // empty directory
    p = filename;          // point to begin of file name
  } else {
    *p = 0;          // overwrite slash
    p += 1;          // point to begin of file name
  }

	// try to open file

	fp = fopen( filename, "r");
	if (!fp) {

		fprintf( stderr, "ERROR: cannot open file '%s'\n", filename);

		return( NULL);
	}

	if (!fgets(line, sizeof line, fp)) {

		fprintf(stderr, "cannot load %s: read error\n", filename);

		fclose( fp);
		return( NULL);
	}

	if (memcmp(line, IQE_MAGIC, strlen(IQE_MAGIC))) {

		fprintf( stderr, "cannot load %s: bad iqe magic\n", filename);

		fclose( fp);
		return( NULL);
	}

	// allocated some data

	struct skel *skel = (struct skel *)malloc(sizeof *skel);
	struct mesh *mesh = (struct mesh *)malloc(sizeof *mesh);
	struct anim *anim = NULL;

	int pose_count = 0;
	struct pose *pose;

	skel->joint_count = 0;
	pose = skel->pose;

	int anim_count = 0;

	// load the data ...

	while (1) {
		float x, y, z, w;
		int a, b, c, d;

		if (!fgets(line, sizeof line, fp))
			break;

		sp = line;

		s = parseword(&sp);
		if (!s)
			continue;

		if (s[0] == 'v' && s[1] != 0 && s[2] == 0) {
			switch (s[1]) {
			case 'p':
				x = parsefloat(&sp, 0);
				y = parsefloat(&sp, 0);
				z = parsefloat(&sp, 0);
				addposition(x, y, z);
				break;

			case 'n':
				x = parsefloat(&sp, 0);
				y = parsefloat(&sp, 0);
				z = parsefloat(&sp, 0);
				addnormal(x, y, z);
				break;

			case 't':
				x = parsefloat(&sp, 0);
				y = parsefloat(&sp, 0);
				addtexcoord(x, y);
				break;

			case 'c':
				x = parsefloat(&sp, 0);
				y = parsefloat(&sp, 0);
				z = parsefloat(&sp, 0);
				w = parsefloat(&sp, 1);
				addcolor(x, y, z, w);
				break;

			case 'b':
				a = parseint(&sp, 0);
				x = parsefloat(&sp, 1);
				b = parseint(&sp, 0);
				y = parsefloat(&sp, 0);
				c = parseint(&sp, 0);
				z = parsefloat(&sp, 0);
				d = parseint(&sp, 0);
				w = parsefloat(&sp, 0);
				addblend(a, b, c, d, x, y, z, w);
				break;
			}
		}

		else if (s[0] == 'f' && s[1] == 'm' && s[2] == 0) {
			a = parseint(&sp, 0);
			b = parseint(&sp, 0);
			c = parseint(&sp, -1);
			while (c > -1) {
				addtriangleFlipWinding(a+fm, b+fm, c+fm);
				b = c;
				c = parseint(&sp, -1);
			}
		}

		else if (s[0] == 'p' && s[1] == 'q' && s[2] == 0) {
			if (pose_count < MAXBONE) {
				pose[pose_count].location[0] = parsefloat(&sp, 0);
				pose[pose_count].location[1] = parsefloat(&sp, 0);
				pose[pose_count].location[2] = parsefloat(&sp, 0);
				pose[pose_count].rotation[0] = parsefloat(&sp, 0);
				pose[pose_count].rotation[1] = parsefloat(&sp, 0);
				pose[pose_count].rotation[2] = parsefloat(&sp, 0);
				pose[pose_count].rotation[3] = parsefloat(&sp, 1);
				pose[pose_count].scale[0] = parsefloat(&sp, 1);
				pose[pose_count].scale[1] = parsefloat(&sp, 1);
				pose[pose_count].scale[2] = parsefloat(&sp, 1);
				pose_count++;
			}
		}

		else if (!strcmp(s, "joint")) {
			if (skel->joint_count < MAXBONE) {
				skel->name[skel->joint_count] = strdup(parsestring(&sp));
				skel->parent[skel->joint_count] = parseint(&sp, -1);
				skel->joint_count++;
			}
		}

		else if (!strcmp(s, "animation")) {
			s = parsestring(&sp);
			anim = pushanim(anim, s);

			anim_count += 1;
		}

		else if (!strcmp(s, "frame")) {
			pose = pushframe(anim, skel->joint_count);
			pose_count = 0;
		}

		else if (!strcmp(s, "framerate")) {

      if( anim != NULL) {   // security test

        anim->framerate = parsefloat(&sp, 10.0);
        if( anim->framerate <= 0.01) {  // security test
          anim->framerate = 10.0;
        }
      }
		}

		else if (!strcmp(s, "mesh")) {
			if (element.len > first)
				pushpart(&partbuf, first, element.len, material, MaterialString, basedir);
			first = element.len;
			fm = position.len / 3;
		}

		else if (!strcmp(s, "material")) {
			s = parsestring(&sp);
			strncpy( MaterialString, s, sizeof( MaterialString) - 1);
			material = -1;              // set no material loaded
		}
	}

	if (element.len > first)
		pushpart(&partbuf, first, element.len, material, MaterialString, basedir);

  // finish

  fclose( fp);

	if (skel->joint_count > 0) {
		calc_matrix_from_pose(loc_bind_matrix, skel->pose, skel->joint_count);
		calc_abs_matrix(mesh->abs_bind_matrix, loc_bind_matrix, skel->parent, skel->joint_count);
		calc_inv_matrix(mesh->inv_bind_matrix, mesh->abs_bind_matrix, skel->joint_count);
	}

	mesh->vertex_count = position.len / 3;
	mesh->position = (float *)duparray(position.data, position.len, sizeof(float));
	mesh->normal = (float *)duparray(normal.data, normal.len, sizeof(float));
	mesh->texcoord = (float *)duparray(texcoord.data, texcoord.len, sizeof(float));
	mesh->color = (float *)duparray(color.data, color.len, sizeof(float));
	mesh->blendindex = (int *)duparray(blendindex.data, blendindex.len, sizeof(int));
	mesh->blendweight = (float *)duparray(blendweight.data, blendweight.len, sizeof(float));
	mesh->aposition = NULL;
	mesh->anormal = NULL;

	mesh->element_count = element.len;
	mesh->element = (int *)duparray(element.data, element.len, sizeof(int));

	mesh->part_count = partbuf.len;
	mesh->part = (part *)duparray(partbuf.data, partbuf.len, sizeof(struct part));

	fprintf(stderr, "\t%d batches; %d vertices; %d triangles; %d joints\n",
			mesh->part_count, mesh->vertex_count, mesh->element_count / 3, skel->joint_count);

	struct model *model = (struct model *)malloc(sizeof *model);
	model->skel = skel;
	model->mesh = mesh;
	// animation data
	if( anim_count > 0 && skel->joint_count > 0) {  // have animation and a skelton
	  model->anim_poses = skel->joint_count;        // set number of poses in an animation frame
	} else {
	  model->anim_poses = 0;                        // reset number of poses in an animation frame
	}
	model->anim_count = anim_count;                 // set number of animations
	model->anim = anim;

  fprintf(stderr, "%s: Done loading\n", filename);

	return model;
}

/************************************************************************************
 * IQM_loadmodel()
 */

static struct model *IQM_loadmodel(char *filename)
{
	static mat4 loc_bind_matrix[MAXBONE];

  char basedir[ 1024];
	FILE *fp = NULL;
	char *p;
  unsigned char *buf = NULL;
  iqmheader hdr;
	struct skel *skel = NULL;
	struct mesh *mesh = NULL;
	struct anim *anim = NULL;
	struct model *model = NULL;
	char *texts;
	iqmvertexarray *vas;

	int anim_count;


	fprintf( stderr, "loading iqm model '%s'\n", filename);

	position.len = 0;
	normal.len = 0;
	texcoord.len = 0;
	color.len = 0;
	blendindex.len = 0;
	blendweight.len = 0;
	element.len = 0;
  partbuf.len = 0;

  // get base directory (path to filename)

	strcpy(basedir, filename);
	p = strrchr(basedir, '/');
	if (!p) p = strrchr(basedir, '\\');
	if (!p) {
    strcpy(basedir, "");    // empty directory
    p = filename;          // point to begin of file name
  } else {
    *p = 0;          // overwrite slash
    p += 1;          // point to begin of file name
  }

	// try to open file

	fp = fopen( filename, "rb");
	if (!fp) {

		fprintf( stderr, "ERROR: cannot open file '%s'\n", filename);

		return( NULL);
	}

  if( fread( &hdr, 1, sizeof(hdr), fp) != sizeof(hdr) || memcmp(hdr.magic, IQM_MAGIC, sizeof(hdr.magic))) {
    goto error;
  }
//   hdr.version = LittleLong( hdr.version );
  if(hdr.version != IQM_VERSION) {
      goto error;
  }

	// byteswap header
	// hdr.filesize = LittleLong(hdr.filesize);
	// hdr.flags = LittleLong(hdr.flags);
	// hdr.num_text = LittleLong(hdr.num_text);
	// hdr.ofs_text = LittleLong(hdr.ofs_text);
	// hdr.num_meshes = LittleLong(hdr.num_meshes);
	// hdr.ofs_meshes = LittleLong(hdr.ofs_meshes);
	// hdr.num_vertexarrays = LittleLong(hdr.num_vertexarrays);
	// hdr.num_vertexes = LittleLong(hdr.num_vertexes);
	// hdr.ofs_vertexarrays = LittleLong(hdr.ofs_vertexarrays);
	// hdr.num_triangles = LittleLong(hdr.num_triangles);
	// hdr.ofs_triangles = LittleLong(hdr.ofs_triangles);
	// hdr.ofs_adjacency = LittleLong(hdr.ofs_adjacency);
	// hdr.num_joints = LittleLong(hdr.num_joints);
	// hdr.ofs_joints = LittleLong(hdr.ofs_joints);
	// hdr.num_poses = LittleLong(hdr.num_poses);
	// hdr.ofs_poses = LittleLong(hdr.ofs_poses);
	// hdr.num_anims = LittleLong(hdr.num_anims);
	// hdr.ofs_anims = LittleLong(hdr.ofs_anims);
	// hdr.num_frames = LittleLong(hdr.num_frames);
	// hdr.num_framechannels = LittleLong(hdr.num_framechannels);
	// hdr.ofs_frames = LittleLong(hdr.ofs_frames);
	// hdr.ofs_bounds = LittleLong(hdr.ofs_bounds);
	// hdr.num_comment = LittleLong(hdr.num_comment);
	// hdr.ofs_comment = LittleLong(hdr.ofs_comment);
	// hdr.num_extensions = LittleLong(hdr.num_extensions);
	// hdr.ofs_extensions = LittleLong(hdr.ofs_extensions);

  if(hdr.filesize > (16<<20)) {
    goto error; // sanity check... don't load files bigger than 16 MB
  }

  buf = (unsigned char *)malloc( hdr.filesize);
  if( fread(buf + sizeof(hdr), 1, hdr.filesize - sizeof(hdr), fp) != hdr.filesize - sizeof(hdr)) {
    goto error;
  }

  // pointer to string data

  texts = hdr.ofs_text ? (char *)&buf[hdr.ofs_text] : (char *)"";

	// allocated some data

	skel = (struct skel *)malloc(sizeof *skel);
	mesh = (struct mesh *)malloc(sizeof *mesh);
	anim = NULL;

	anim_count = 0;

	skel->joint_count = 0;

	// load the data ...

	unsigned int iLoop;
	float *pInFloat;
	unsigned char *pUByte;
	int *pInInt;

  vas = (iqmvertexarray *)&buf[hdr.ofs_vertexarrays];
  for( int iVarray = 0; iVarray < (int)hdr.num_vertexarrays; iVarray++)
  {
    iqmvertexarray &va = vas[ iVarray];

	// 	va.type = LittleLong( va.type );
  	// va.flags = LittleLong( va.flags );
	//   va.format = LittleLong( va.format );
	//   va.size = LittleLong( va.size );
	//   va.offset = LittleLong( va.offset );

    switch(va.type)
    {
    case IQM_POSITION:
      if(va.format != IQM_FLOAT || va.size != 3) {
        goto error;
      }
      pInFloat = (float *)( buf + va.offset);
	    for( iLoop = 0; iLoop < hdr.num_vertexes; iLoop++ ) {
        // pInFloat[0] = LittleFloat( pInFloat[0] );
        // pInFloat[1] = LittleFloat( pInFloat[1] );
        // pInFloat[2] = LittleFloat( pInFloat[2] );
    		addposition( pInFloat[0], pInFloat[1], pInFloat[2]);
	      pInFloat += 3;
	    }
      break;
    case IQM_NORMAL:
      if(va.format != IQM_FLOAT || va.size != 3) {
        goto error;
      }
      pInFloat = (float *)( buf + va.offset);
	    for( iLoop = 0; iLoop < hdr.num_vertexes; iLoop++ ) {
        // pInFloat[0] = LittleFloat( pInFloat[0] );
        // pInFloat[1] = LittleFloat( pInFloat[1] );
        // pInFloat[2] = LittleFloat( pInFloat[2] );
  			addnormal( pInFloat[0], pInFloat[1], pInFloat[2]);
		    pInFloat += 3;
	    }
      break;
    case IQM_TANGENT:
      if(va.format != IQM_FLOAT || va.size != 4) {
        goto error;
      }
      // is not used
      break;
    case IQM_TEXCOORD:
      if(va.format != IQM_FLOAT || va.size != 2) {
        goto error;
      }
      pInFloat = (float *)( buf + va.offset);
	    for( iLoop = 0; iLoop < hdr.num_vertexes; iLoop++ ) {
        // pInFloat[0] = LittleFloat( pInFloat[0] );
        // pInFloat[1] = LittleFloat( pInFloat[1] );
  	  	addtexcoord( pInFloat[0], pInFloat[1]);
		    pInFloat += 2;
      }
      break;
    case IQM_BLENDINDEXES:
      if(va.format != IQM_UBYTE || va.size != 4) {
        goto error;
      }
      pUByte = (unsigned char *)( buf + va.offset);
	    for( iLoop = 0; iLoop < hdr.num_vertexes; iLoop++ ) {
      	pushint(&blendindex, pUByte[0]);
       	pushint(&blendindex, pUByte[1]);
       	pushint(&blendindex, pUByte[2]);
       	pushint(&blendindex, pUByte[3]);
		    pUByte += 4;
	    }
      break;
    case IQM_BLENDWEIGHTS:
      if(va.format != IQM_UBYTE || va.size != 4) {
        goto error;
      }
      pUByte = (unsigned char *)( buf + va.offset);
	    for( iLoop = 0; iLoop < hdr.num_vertexes; iLoop++ ) {

        float total = pUByte[0] + pUByte[1] + pUByte[2] + pUByte[3];

      	pushfloat(&blendweight, pUByte[0] / total);
       	pushfloat(&blendweight, pUByte[1] / total);
       	pushfloat(&blendweight, pUByte[2] / total);
       	pushfloat(&blendweight, pUByte[3] / total);
		    pUByte += 4;
	    }
      break;
    case IQM_COLOR:
      if(va.format != IQM_UBYTE || va.size != 4) {
        goto error;
      }
      pUByte = (unsigned char *)( buf + va.offset);
	    for( iLoop = 0; iLoop < hdr.num_vertexes; iLoop++ ) {
  	  	addcolor( pUByte[0] / 255.0, pUByte[1] / 255.0, pUByte[2] / 255.0, pUByte[3] / 255.0);
        pUByte += 4;
      }
      break;
    }
  }

	// load triangles

  pInInt = (int *)( buf + hdr.ofs_triangles);
	for( iLoop = 0; iLoop < hdr.num_triangles; iLoop++ ) {
    // pInInt[0] = LittleLong( pInInt[0] );
    // pInInt[1] = LittleLong( pInInt[1] );
    // pInInt[2] = LittleLong( pInInt[2] );
    addtriangleFlipWinding( pInInt[0], pInInt[1], pInInt[2]);
    pInInt += 3;
  }

	// load meshes

  iqmmesh *inmesh;
  inmesh = (iqmmesh *)( buf + hdr.ofs_meshes);

	for( iLoop = 0; iLoop < hdr.num_meshes; iLoop++ ) {

    int material;

		// inmesh[iLoop].name = LittleLong( inmesh[iLoop].name );
		// inmesh[iLoop].material = LittleLong( inmesh[iLoop].material );
		// inmesh[iLoop].first_vertex = LittleLong( inmesh[iLoop].first_vertex );
		// inmesh[iLoop].num_vertexes = LittleLong( inmesh[iLoop].num_vertexes );
		// inmesh[iLoop].first_triangle = LittleLong( inmesh[iLoop].first_triangle );
		// inmesh[iLoop].num_triangles = LittleLong( inmesh[iLoop].num_triangles );

		//x/poutmodel->meshes[iLoop].name = texts + inmesh[Loopi].name;
		//x/pSkinNameOriginal = texts + inmesh[iLoop].material;

		material = -1;   // no material loaded
		pushpart( &partbuf, inmesh[iLoop].first_triangle * 3, (inmesh[iLoop].first_triangle + inmesh[iLoop].num_triangles) * 3,
              material, texts + inmesh[iLoop].material, basedir);

	}

  // load joints (the skeleton)

	iqmjoint *joints;

  joints = (iqmjoint *)( buf + hdr.ofs_joints);

	for( iLoop = 0; iLoop < hdr.num_joints; iLoop++ ) {

    int jLoop;

		// joints[iLoop].name = LittleLong( joints[iLoop].name );
		// joints[iLoop].parent = LittleLong( joints[iLoop].parent );

		if( joints[iLoop].parent >= (int)iLoop) {
			//x/Com_Error( ERR_DROP, "%s bone[%i].parent(%i) >= %i", mod->name, iLoop, joints[iLoop].parent, iLoop);

			goto error;
		}

		if (skel->joint_count < MAXBONE) {
			skel->name[skel->joint_count] = strdup( texts + joints[iLoop].name);
			skel->parent[skel->joint_count] = joints[iLoop].parent;

  		for( jLoop = 0; jLoop < 3; jLoop++ ) {
	  		// joints[iLoop].translate[jLoop] = LittleFloat( joints[iLoop].translate[jLoop] );
 			  skel->pose[skel->joint_count].location[jLoop] = joints[iLoop].translate[jLoop];

			//   joints[iLoop].scale[jLoop] = LittleFloat( joints[iLoop].scale[jLoop] );
 			  skel->pose[skel->joint_count].scale[jLoop] = joints[iLoop].scale[jLoop];
		  }

  		for( jLoop = 0; jLoop < 4; jLoop++ ) {
		  	// joints[iLoop].rotate[jLoop] = LittleFloat( joints[iLoop].rotate[jLoop] );
 			  skel->pose[skel->joint_count].rotation[jLoop] = joints[iLoop].rotate[jLoop];
  		}

			skel->joint_count++;
		}
	}

	// load animations

  unsigned int nFrames;
  struct pose **FrameDataTable;

  FrameDataTable = NULL;
  nFrames = 0;
  if( hdr.num_frames > 0) {

	  FrameDataTable = (struct pose **)malloc( hdr.num_frames * sizeof(*FrameDataTable));
  }

  // ...

	struct iqmanim *anims;

	anims = ( iqmanim * )( buf + hdr.ofs_anims);

	for( iLoop = 0; iLoop < hdr.num_anims; iLoop++ ) {

    unsigned int jLoop;

		// anims[iLoop].name = LittleLong( anims[iLoop].name);
		// anims[iLoop].first_frame = LittleLong( anims[iLoop].first_frame);
		// anims[iLoop].num_frames = LittleLong( anims[iLoop].num_frames);
		// anims[iLoop].framerate = LittleFloat( anims[iLoop].framerate);
		// anims[iLoop].flags = LittleLong( anims[iLoop].flags);

    anim = pushanim( anim, texts + anims[iLoop].name);

    anim->len = anims[iLoop].num_frames;
    anim->framerate = anims[iLoop].framerate;
    if( anim->framerate <= 0.01) {  // security test
      anim->framerate = 10.0;
    }

    anim_count++;

    // allocate memory for the animations

    if( anims[iLoop].num_frames > 0) {

		  anim->data = (struct pose **)malloc( anims[iLoop].num_frames * sizeof(*anim->data));
    }

    for( jLoop = 0; jLoop < anims[iLoop].num_frames; jLoop++) {

 	    anim->data[ jLoop] = (struct pose *)malloc( sizeof(struct pose) * hdr.num_poses);

      // where to store the loaded frame data
      if( nFrames < hdr.num_frames) {

        FrameDataTable[ nFrames] = anim->data[ jLoop];
        nFrames++;
      }
   }

    // NOTE: loop flag is not read in until know
  }

	// load frames

	struct iqmpose *poses;

	poses = ( iqmpose * )( buf + hdr.ofs_poses);

	for( iLoop = 0; iLoop < hdr.num_poses; iLoop++ ) {

    int jLoop;

		// poses[iLoop].parent = LittleLong( poses[iLoop].parent );
		// poses[iLoop].mask = LittleLong( poses[iLoop].mask );

		for( jLoop = 0; jLoop < 10; jLoop++ ) {
			// poses[iLoop].channeloffset[jLoop] = LittleFloat( poses[iLoop].channeloffset[jLoop] );
			// poses[iLoop].channelscale[jLoop] = LittleFloat( poses[iLoop].channelscale[jLoop] );
		}
	}

	unsigned short *framedata;

	framedata = ( unsigned short * )( buf + hdr.ofs_frames);

	for( iLoop = 0; iLoop < hdr.num_frames; iLoop++ ) {
    unsigned int j;
    struct pose *pOutPose, *pbp;


		pOutPose = FrameDataTable[ iLoop];

		for( j = 0, pbp = pOutPose; j < hdr.num_poses; j++, pbp++ ) {

			pbp->location[0] = poses[j].channeloffset[0]; if( poses[j].mask & 0x01 ) pbp->location[0] += *framedata++ * poses[j].channelscale[0];
			pbp->location[1] = poses[j].channeloffset[1]; if( poses[j].mask & 0x02 ) pbp->location[1] += *framedata++ * poses[j].channelscale[1];
			pbp->location[2] = poses[j].channeloffset[2]; if( poses[j].mask & 0x04 ) pbp->location[2] += *framedata++ * poses[j].channelscale[2];

			pbp->rotation[0] = poses[j].channeloffset[3]; if( poses[j].mask & 0x08 ) pbp->rotation[0] += *framedata++ * poses[j].channelscale[3];
			pbp->rotation[1] = poses[j].channeloffset[4]; if( poses[j].mask & 0x10 ) pbp->rotation[1] += *framedata++ * poses[j].channelscale[4];
			pbp->rotation[2] = poses[j].channeloffset[5]; if( poses[j].mask & 0x20 ) pbp->rotation[2] += *framedata++ * poses[j].channelscale[5];
			pbp->rotation[3] = poses[j].channeloffset[6]; if( poses[j].mask & 0x40 ) pbp->rotation[3] += *framedata++ * poses[j].channelscale[6];

			if( pbp->rotation[3] > 0 ) {
				Vector4Inverse( pbp->rotation);
			}
			Vector4Normalize( pbp->rotation);

#ifdef use_again
			// scale is unused
			if( poses[j].mask & 0x80  ) framedata++;
			if( poses[j].mask & 0x100 ) framedata++;
			if( poses[j].mask & 0x200 ) framedata++;
#else
			pbp->scale[0] = poses[j].channeloffset[7]; if( poses[j].mask & 0x080 ) pbp->scale[0] += *framedata++ * poses[j].channelscale[7];
			pbp->scale[1] = poses[j].channeloffset[8]; if( poses[j].mask & 0x100 ) pbp->scale[1] += *framedata++ * poses[j].channelscale[8];
			pbp->scale[2] = poses[j].channeloffset[9]; if( poses[j].mask & 0x200 ) pbp->scale[2] += *framedata++ * poses[j].channelscale[9];
#endif
		}
	}

  // finish

  free( buf);         // free data of model

  fclose( fp);
  fp = NULL;          // flag that fp is closed

	if (skel->joint_count > 0) {
		calc_matrix_from_pose(loc_bind_matrix, skel->pose, skel->joint_count);
		calc_abs_matrix(mesh->abs_bind_matrix, loc_bind_matrix, skel->parent, skel->joint_count);
		calc_inv_matrix(mesh->inv_bind_matrix, mesh->abs_bind_matrix, skel->joint_count);
	}

	mesh->vertex_count = position.len / 3;
	mesh->position = (float *)duparray(position.data, position.len, sizeof(float));
	mesh->normal = (float *)duparray(normal.data, normal.len, sizeof(float));
	mesh->texcoord = (float *)duparray(texcoord.data, texcoord.len, sizeof(float));
	mesh->color = (float *)duparray(color.data, color.len, sizeof(float));
	mesh->blendindex = (int *)duparray(blendindex.data, blendindex.len, sizeof(int));
	mesh->blendweight = (float *)duparray(blendweight.data, blendweight.len, sizeof(float));
	mesh->aposition = NULL;
	mesh->anormal = NULL;

	mesh->element_count = element.len;
	mesh->element = (int *)duparray(element.data, element.len, sizeof(int));

	mesh->part_count = partbuf.len;
	mesh->part = (part *)duparray(partbuf.data, partbuf.len, sizeof(struct part));

	fprintf(stderr, "\t%d batches; %d vertices; %d triangles; %d bones\n",
			mesh->part_count, mesh->vertex_count, mesh->element_count / 3, skel->joint_count);

	model = (struct model *)malloc(sizeof *model);
	model->skel = skel;
	model->mesh = mesh;
	if( anim_count > 0 && hdr.num_poses > 0) {      // have animation and poses
	  model->anim_poses = hdr.num_poses;            // set number of poses in an animation frame
	} else {
	  model->anim_poses = 0;                        // reset number of poses in an animation frame
	}
	model->anim_count = anim_count;                 // set number of animations
	model->anim = anim;

  fprintf(stderr, "%s: Done loading\n", filename);

	return model;

error:
	fprintf( stderr, "%s: ERROR while loading\n", filename);
  if( buf != NULL) {
    free( buf);
  }
  if( skel != NULL) {
    free( skel);
  }
  if( mesh != NULL) {
    free( mesh);
  }
  if( anim != NULL) {
    free( anim);
  }

  if( fp != NULL) {
    fclose( fp);
  }
  return NULL;
}

/************************************************************************************
 * freemodel()
 *
 * Free's all data realeased by a model.
 */

static void freemodel( struct model **ppModel)
{
  int i, j;
  struct model *model;
  struct anim *anim, *next;

  model = *ppModel;            // Get pointer to model pointer

  if( model == NULL) {         /* security test */

    return;
  }

  *ppModel = NULL;             // reset pointer to model pointer

  //

  if( pDrawModel == model) {   // this model was used for display

    pDrawModel = NULL;         // data is no valid so invalidate pointer
  }

  // release mesh texture and strings

	for( i = 0; i < model->mesh->part_count; i++) {

    if( model->mesh->part[i].material > 0) {       // openGL has loaded

      unsigned int TempMaterial;

      TempMaterial = model->mesh->part[i].material;

    //   glDeleteTextures( 1, &TempMaterial);
    }

    model->mesh->part[i].material = -1;            // invalidate loaded texture

    // ...

    free( model->mesh->part[ i].pMaterialString);
    free( model->mesh->part[ i].pMaterialBasedir);
	}

  // free mesh

	if( model->mesh->position)   free( model->mesh->position);
	if( model->mesh->normal)   free( model->mesh->normal);
	if( model->mesh->texcoord)   free( model->mesh->texcoord);
	if( model->mesh->color)   free( model->mesh->color);
	if( model->mesh->blendindex)   free( model->mesh->blendindex);
	if( model->mesh->blendweight)   free( model->mesh->blendweight);
	if( model->mesh->element)   free( model->mesh->element);
	if( model->mesh->part)   free( model->mesh->part);
	if( model->mesh->aposition) free( model->mesh->aposition);
	if( model->mesh->anormal)   free( model->mesh->anormal);

	// free animations

	anim = model->anim;
	model->anim = NULL;
	while( anim != NULL) {

    next = anim->next;         // save this before free of data

    free( anim->name);

    if( anim->data) {

      for( j = 0; j < anim->len; j++) {

        free( anim->data[ j]);
      }

      free( anim->data);
    }

    free( anim);

    anim = next;
	}

	// free skeleton

	for( i = 0; i < model->skel->joint_count; i++) {

    free( model->skel->name[ i]);
	}

	// ...

	if( model->skel)   free( model->skel);
	if( model->mesh)   free( model->mesh);
	if( model->anim)   free( model->anim);


  free( model);
}

/************************************************************************************
 * copymodel()
 *
 * get a copy of a model
 */

#define COPYMODEL_COPY_ALL     0   // copy everything, distination model is allocated
#define COPYMODEL_SKELETON     1   // copy skeleton, distination must be allocated and have no skeleton
#define COPYMODEL_MESH         2   // copy skeleton, distination must be allocated and have no mesh, vertexes, trigangle
#define COPYMODEL_ANIMATION    4   // copy animation, distination must be allocated, append animation
#define COPYMODEL_ALL_PARTS ( COPYMODEL_SKELETON | COPYMODEL_MESH | COPYMODEL_ANIMATION) // copy all parts, distination must be allocated

static void copymodel( struct model **ppModelSrc, struct model **ppModelDst, int CopyFlags)
{
  int i, j;
  struct model *pModelSrc, *pModelDst;

  pModelSrc = *ppModelSrc;     // Get pointer to model pointer
  if( pModelSrc == NULL) {     // security test, we need a source

    return;
  }

  if( CopyFlags == COPYMODEL_COPY_ALL) {  // allocatd complete destination model

    freemodel( ppModelDst);               // enshure there is no destination model

	  pModelDst = (struct model *)malloc(sizeof *pModelDst);  // allocate

  	memset( pModelDst,       0, sizeof( struct model));

	  pModelDst->skel = (struct skel *)malloc(sizeof *pModelDst->skel);
	  memset( pModelDst->skel, 0, sizeof( struct skel));

	  pModelDst->mesh = (struct mesh *)malloc(sizeof *pModelDst->mesh);
	  memset( pModelDst->mesh, 0, sizeof( struct mesh));

    CopyFlags = COPYMODEL_ALL_PARTS;      // continue copy all parts
  } else {

    pModelDst = *ppModelDst;              // Get pointer to model pointer
  }

  if( (CopyFlags & COPYMODEL_MESH) != 0) {  // copy mesh

    int VertexCount;

    VertexCount = pModelSrc->mesh->vertex_count;
	  pModelDst->mesh->vertex_count = VertexCount;
	  pModelDst->mesh->position     = (float *)duparray( pModelSrc->mesh->position,    VertexCount * 3, sizeof(float));
	  pModelDst->mesh->normal       = (float *)duparray( pModelSrc->mesh->normal,      VertexCount * 3, sizeof(float));
	  pModelDst->mesh->texcoord     = (float *)duparray( pModelSrc->mesh->texcoord,    VertexCount * 2, sizeof(float));
	  pModelDst->mesh->color        = (float *)duparray( pModelSrc->mesh->color,       VertexCount * 4, sizeof(float));
	  pModelDst->mesh->blendindex   =   (int *)duparray( pModelSrc->mesh->blendindex,  VertexCount * 4, sizeof(int));
	  pModelDst->mesh->blendweight  = (float *)duparray( pModelSrc->mesh->blendweight, VertexCount * 4, sizeof(float));
	  pModelDst->mesh->aposition    = NULL;
	  pModelDst->mesh->anormal      = NULL;

	  pModelDst->mesh->element_count = pModelSrc->mesh->element_count;
	  pModelDst->mesh->element = (int *)duparray( pModelSrc->mesh->element, pModelSrc->mesh->element_count, sizeof(int));

	  pModelDst->mesh->part_count = pModelSrc->mesh->part_count;
	  pModelDst->mesh->part = (part *)duparray( pModelSrc->mesh->part, pModelSrc->mesh->part_count, sizeof(struct part));

    // handle partial mesh things

	  for( i = 0; i < pModelDst->mesh->part_count; i++) {

      pModelDst->mesh->part[i].material = -1;            // invalidate loaded texture

      // ...

      pModelDst->mesh->part[ i].pMaterialString  = strdup( pModelSrc->mesh->part[ i].pMaterialString);
      pModelDst->mesh->part[ i].pMaterialBasedir = strdup( pModelSrc->mesh->part[ i].pMaterialBasedir);
	  }
  }

	// copy skeleton things

  if( (CopyFlags & COPYMODEL_SKELETON) != 0) {  // copy skeleton

	  pModelDst->skel = (skel *)duparray( pModelSrc->skel, 1, sizeof(struct skel));

	  for( i = 0; i < pModelDst->skel->joint_count; i++) {

      pModelDst->skel->name[ i] = strdup( pModelSrc->skel->name[ i]);
	  }

	  memcpy( pModelDst->mesh->abs_bind_matrix, pModelSrc->mesh->abs_bind_matrix, sizeof( pModelDst->mesh->abs_bind_matrix));
	  memcpy( pModelDst->mesh->inv_bind_matrix, pModelSrc->mesh->inv_bind_matrix, sizeof( pModelDst->mesh->inv_bind_matrix));
  }

  // copy animations

  if( (CopyFlags & COPYMODEL_ANIMATION) != 0) {  // copy/append animation

	  int anim_poses, anim_count;
	  struct anim *animSrc, *animDst;

  	if( pModelSrc->anim_count > 0 && pModelSrc->anim_poses > 0) {  // have animation and poses
	    anim_poses = pModelSrc->anim_poses;    // set number of poses in an animation frame
	  } else {
	    anim_poses = 0;                        // reset number of poses in an animation frame
	  }

	  anim_count = 0;

	  animSrc = pModelSrc->anim;
	  animDst = NULL;
  	while( animSrc != NULL && anim_poses > 0) {

      animDst = pushanim( animDst, animSrc->name);
      animDst->len = animSrc->len;
      animDst->framerate = animSrc->framerate;

      anim_count++;

      // allocate memory for the animations

      if( animDst->len > 0) {

		    animDst->data = (struct pose **)malloc( animDst->len * sizeof(*animDst->data));
      }

      for( j = 0; j < animDst->len; j++) {

 	      animDst->data[ j] = (struct pose *)duparray( animSrc->data[ j], 1, sizeof(struct pose) * anim_poses);
      }

      animSrc = animSrc->next;         // next in chain
    }

	  pModelDst->anim_poses = anim_poses;
	  pModelDst->anim_count = anim_count;
	  pModelDst->anim = animDst;
  }

	// finish up

  *ppModelDst = pModelDst;    // set pointer to model

}

/************************************************************************************
 * animatemodel()
 */

static mat4 loc_pose_matrix[MAXBONE];
static mat4 abs_pose_matrix[MAXBONE];
static mat4 skin_matrix[MAXBONE];

void animatemodel(struct model *model, struct anim *anim, int frame)
{
	struct skel *skel = model->skel;
	struct mesh *mesh = model->mesh;

	frame = CLAMP(frame, 0, anim->len-1);

	calc_matrix_from_pose(loc_pose_matrix, anim->data[frame], skel->joint_count);
	calc_abs_matrix(abs_pose_matrix, loc_pose_matrix, skel->parent, skel->joint_count);
	calc_mul_matrix(skin_matrix, abs_pose_matrix, mesh->inv_bind_matrix, skel->joint_count);

	if (!mesh->aposition) mesh->aposition = (float *)malloc(sizeof(float) * mesh->vertex_count * 3);
	if (!mesh->anormal) mesh->anormal = (float *)malloc(sizeof(float) * mesh->vertex_count * 3);

	int *bi = mesh->blendindex;
	float *bw = mesh->blendweight;
	float *sp = mesh->position;
	float *sn = mesh->normal;
	float *dp = mesh->aposition;
	float *dn = mesh->anormal;
	int n = mesh->vertex_count;

	while (n--) {
		int i;
		dp[0] = dp[1] = dp[2] = 0;
		dn[0] = dn[1] = dn[2] = 0;
		for (i = 0; i < 4; i++) {
			vec3 tp, tn;

			if( bw[i] == 0.0) {   // and of list
        break;
			}

			mat_vec_mul(tp, skin_matrix[bi[i]], sp);
			mat_vec_mul_n(tn, skin_matrix[bi[i]], sn);
			vec_scale(tp, tp, bw[i]);
			vec_scale(tn, tn, bw[i]);
			vec_add(dp, dp, tp);
			vec_add(dn, dn, tn);
		}
		bi += 4; bw += 4;
		sp += 3; sn += 3;
		dp += 3; dn += 3;
	}
}

/************************************************************************************
 * drawskeleton()
 */

static void getPixel3dTo2D( float x, float y, float z, int *pOutX, int *pOutY)
{
//   GLdouble modelMatrix[16];
//   GLdouble projMatrix[16];
//   GLint viewport[4];

//   GLdouble objx = x;
//   GLdouble objy = y;
//   GLdouble objz = z;

//   GLdouble pix_x;
//   GLdouble pix_y;
//   GLdouble pix_z;

//   memset( modelMatrix, 0, sizeof( modelMatrix));
//   memset( projMatrix, 0, sizeof( projMatrix));
//   memset( viewport, 0, sizeof( viewport));

//   glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
//   glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
//   glGetIntegerv(GL_VIEWPORT, viewport);

//   gluProject( objx, objy, objz, modelMatrix, projMatrix, viewport,
//               &pix_x, &pix_y, &pix_z);

//   //need to reverse y since OpenGL has y=0 at bottom, but windows has y=0 at top
//   *pOutX = (int)pix_x - viewport[0];
//   *pOutY = viewport[1] + viewport[3] - (int)pix_y;
}

static int haschildren(int *parent, int count, int x)
{
	int i;
	for (i = x; i < count; i++)
		if (parent[i] == x)
			return 1;
	return 0;
}

void drawskeleton(struct model *model)
{
// 	struct skel *skel = model->skel;
// 	vec3 x = { 0, 0.1, 0 };
// 	int i;
// 	glBegin(GL_LINES);
// 	for (i = 0; i < skel->joint_count; i++) {
// 		float *a = abs_pose_matrix[i];
// 		if (skel->parent[i] >= 0) {
// 			float *b = abs_pose_matrix[skel->parent[i]];
// 			glColor4f(1, 1, 0, 1);
// 			glVertex3f(a[12], a[13], a[14]);
// 			glVertex3f(b[12], b[13], b[14]);
// 		} else {
// 			glColor4f(1, 1, 0, 1);
// 			glVertex3f(a[12], a[13], a[14]);
// 			glColor4f(0, 0, 0, 1);
// 			glVertex3f(0, 0, 0);
// 		}
// 		if (!haschildren(skel->parent, skel->joint_count, i)) {
// 			vec3 b;
// 			mat_vec_mul(b, abs_pose_matrix[i], x);
// 			glColor4f(1, 1, 0, 1);
// 			glVertex3f(a[12], a[13], a[14]);
// 			glColor4f(0, 0, 0, 1);
// 			glVertex3f(b[0], b[1], b[2]);
// 		}
// 	}

// 	glEnd();

//   // Get 2D positions of joints
//   // NOTE: Must do this outside glBegin()/glEnd(
// 	for (i = 0; i < skel->joint_count; i++) {
// 		float *a = abs_pose_matrix[i];

//     getPixel3dTo2D( a[12], a[13], a[14], skel->TempDrawPosX + i, skel->TempDrawPosY + i);
// 	}

}

/************************************************************************************
 * drawmodel()
 */

void drawmodel(struct model *model)
{
// 	struct mesh *mesh = model->mesh;
// 	int i;

// 	glEnableClientState(GL_VERTEX_ARRAY);
// 	glVertexPointer(3, GL_FLOAT, 0, mesh->aposition ? mesh->aposition : mesh->position);

// 	if (mesh->normal || mesh->anormal) {
//     glEnableClientState(GL_NORMAL_ARRAY);
//     glNormalPointer(GL_FLOAT, 0, mesh->anormal ? mesh->anormal : mesh->normal);
// 	}

// 	if (mesh->texcoord) {
//     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//     glTexCoordPointer(2, GL_FLOAT, 0, mesh->texcoord);
// 	}

// 	if (mesh->color) {
//     glEnableClientState(GL_COLOR_ARRAY);
//     glColorPointer(4, GL_FLOAT, 0, mesh->color);
// 	}

// 	for (i = 0; i < mesh->part_count; i++) {
// 		glColor4f(1, 1, 1, 1);
// 		glBindTexture(GL_TEXTURE_2D, mesh->part[i].material);
// 		glDrawElements(GL_TRIANGLES, mesh->part[i].count, GL_UNSIGNED_INT, mesh->element + mesh->part[i].first);
// 	}

// 	glDisableClientState(GL_VERTEX_ARRAY);
// 	glDisableClientState(GL_NORMAL_ARRAY);
// 	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//   glDisableClientState(GL_COLOR_ARRAY);
}

/************************************************************************************
 * measuremodel()
 */

float measuremodel(struct model *model, float center[3], float size[3])
{
	struct skel *skel = model->skel;
	struct mesh *mesh = model->mesh;
	struct anim *anim;
	float dist, maxdist = 0.25;
	float minbox[3], maxbox[3];
	int i, k, nMinMax;

	center[0] = center[1] = center[2] = 0;

#ifdef use_again
	for (i = 0; i < mesh->vertex_count; i++)
		vec_add(center, center, mesh->position + i * 3);
	if (mesh->vertex_count) {
		center[0] /= mesh->vertex_count;
		center[1] /= mesh->vertex_count;
		center[2] /= mesh->vertex_count;
	}
#else
  // 25.03.2014 RR: use min/max box center, is better than center of gravity

  minbox[0] = minbox[1] = minbox[2] = 0;
  maxbox[0] = maxbox[1] = maxbox[2] = 0;
  nMinMax = 0;

  // vertex

	for (i = 0; i < mesh->vertex_count; i++) {

    if( nMinMax == 0) {

  		vec_copy( minbox, mesh->position + i * 3);
  		vec_copy( maxbox, mesh->position + i * 3);
    } else {

      vec_minmax( minbox, maxbox, mesh->position + i * 3);
    }

    nMinMax += 1;
  }

  // skelton

	if (skel->joint_count > 0) {

  	for (i = 0; i < skel->joint_count; i++) {

      if( nMinMax == 0) {

    		vec_copy( minbox, mesh->abs_bind_matrix[i] + 12);
  	  	vec_copy( maxbox, mesh->abs_bind_matrix[i] + 12);
      } else {

        vec_minmax( minbox, maxbox, mesh->abs_bind_matrix[i] + 12);
      }

      nMinMax += 1;
  	}

		for (anim = model->anim; anim; anim = anim->next) {
			for (k = 0; anim && k < anim->len; k++) {
				calc_matrix_from_pose(loc_pose_matrix, anim->data[k], skel->joint_count);
				calc_abs_matrix(abs_pose_matrix, loc_pose_matrix, skel->parent, skel->joint_count);
				for (i = 0; i < skel->joint_count; i++) {

          if( nMinMax == 0) {

    	  	  vec_copy( minbox, abs_pose_matrix[i] + 12);
  	  	    vec_copy( maxbox, abs_pose_matrix[i] + 12);
          } else {

            vec_minmax( minbox, maxbox, abs_pose_matrix[i] + 12);
          }

          nMinMax += 1;
				}
			}
		}
	}

	center[0] = (minbox[0] + maxbox[0]) * 0.5;
	center[1] = (minbox[1] + maxbox[1]) * 0.5;
	center[2] = (minbox[2] + maxbox[2]) * 0.5;

	size[0] = maxbox[0] - minbox[0];
	size[1] = maxbox[1] - minbox[1];
	size[2] = maxbox[2] - minbox[2];
#endif

	for (i = 0; i < mesh->vertex_count; i++) {
		dist = vec_dist2(center, mesh->position + i * 3);
		if (dist > maxdist)
			maxdist = dist;
	}

	if (skel->joint_count > 0) {
		for (i = 0; i < skel->joint_count; i++) {
			dist = vec_dist2(center, mesh->abs_bind_matrix[i] + 12);
			if (dist > maxdist)
				maxdist = dist;
		}

		for (anim = model->anim; anim; anim = anim->next) {
			for (k = 0; anim && k < anim->len; k++) {
				calc_matrix_from_pose(loc_pose_matrix, anim->data[k], skel->joint_count);
				calc_abs_matrix(abs_pose_matrix, loc_pose_matrix, skel->parent, skel->joint_count);
				for (i = 0; i < skel->joint_count; i++) {
					dist = vec_dist2(center, abs_pose_matrix[i] + 12);
					if (dist > maxdist)
						maxdist = dist;
				}
			}
		}

		memcpy(abs_pose_matrix, mesh->abs_bind_matrix, sizeof abs_pose_matrix);
	}

	return sqrt(maxdist);
}

/************************************************************************************
 * Boring UI and GLUT hooks.
 */

#define DIABLO 36.8698976	// 4:3 isometric view
#define ISOMETRIC 35.264	// true isometric view
#define DIMETRIC 30		// 2:1 'isometric' as seen in pixel art

int showhelp = 1;
int doplane = 0;
int dowire = 0;
int dotexture = 1;
int dobackface = 1;
int doperspective = 1;
int doskeleton = 1;
int doJointNames = 1;
int doplay = 1;

// ...

int screenw = 1024, screenh = 768;
int mousex, mousey, mouseleft = 0, mousemiddle = 0, mouseright = 0;

int gridsize = 3;
float mindist = 1;
float maxdist = 10;

float light_position[4] = { -1, -2, 2, 0 };

typedef struct {
	float distance;
	float yaw;
	float pitch;
	float ObjCenter[3];   // Center of object
	float ObjSize[3];     // size of object
} T_camera;

T_camera camera = { 3, 45, -DIMETRIC, { 0, 1, 0 } };

// setup model to draw

void SetupDrawModel( struct model *pModel)
{
  int i;

  // for draw model in use, unlink textures

  if( pDrawModel != NULL) {

	  for( i = 0; i < pDrawModel->mesh->part_count; i++) {

      if( pDrawModel->mesh->part[i].material > 0) {       // openGL has loaded

        unsigned int TempMaterial;

        TempMaterial = pDrawModel->mesh->part[i].material;

        // glDeleteTextures( 1, &TempMaterial);
      }

      pDrawModel->mesh->part[i].material = -1;            // invalidate loaded texture
	  }
  }

  // model to draw

  pDrawModel = pModel;

  // for draw model in use, load materials

  if( pDrawModel != NULL) {

	  for( i = 0; i < pDrawModel->mesh->part_count; i++) {

      if( pDrawModel->mesh->part[i].material > 0) {       // openGL has loaded

        unsigned int TempMaterial;

        TempMaterial = pDrawModel->mesh->part[i].material;

        // glDeleteTextures( 1, &TempMaterial);
      }

      pDrawModel->mesh->part[i].material = loadmaterial( pDrawModel->mesh->part[i].pMaterialString, pDrawModel->mesh->part[i].pMaterialBasedir);  // load texture
	  }
  }

  // invalidate any message to draw

  strcpy( DrawMessageText1, "");
}

// setup messag to draw

#define DRAW_MSG_COL_ERR 1.0, 0.2, 0.2     // red, error
#define DRAW_MSG_COL_OK  0.1, 0.8, 0.1     // green, OK

void SetupDrawMessage( float r, float g, float b, char *pText1,  char *pText2, char *pText3)
{

  DrawMessageColR = r;
  DrawMessageColG = g;
  DrawMessageColB = b;

  DrawMessageText1[ 0] = '\0';  // reset to empty
  DrawMessageText2[ 0] = '\0';
  DrawMessageText3[ 0] = '\0';

  if( pText1 != NULL) strcpy( DrawMessageText1, pText1);
  if( pText2 != NULL) strcpy( DrawMessageText2, pText2);
  if( pText3 != NULL) strcpy( DrawMessageText3, pText3);
}

// ...

void perspective(float fov, float aspect, float znear, float zfar)
{
	fov = fov * 3.14159 / 360.0;
	fov = tan(fov) * znear;
	// glFrustum(-fov * aspect, fov * aspect, -fov, fov, znear, zfar);
}

void orthogonal(float fov, float aspect, float znear, float zfar)
{
	// glOrtho(-fov * aspect, fov * aspect, -fov, fov, znear, zfar);
}

void drawstring(float x, float y, char *s)
{
	// glRasterPos2f(x+0.375, y+0.375);
	// while (*s)
	// 	//glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s++);
	// 	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *s++);
}

void mouse(int button, int state, int x, int y)
{
	// if (button == GLUT_LEFT_BUTTON) mouseleft = state == GLUT_DOWN;
	// if (button == GLUT_MIDDLE_BUTTON) mousemiddle = state == GLUT_DOWN;
	// if (button == GLUT_RIGHT_BUTTON) mouseright = state == GLUT_DOWN;
	// mousex = x;
	// mousey = y;
}

void motion(int x, int y)
{
	// int dx = x - mousex;
	// int dy = y - mousey;
	// if (mouseleft) {
	// 	camera.yaw -= dx * 0.3;
	// 	camera.pitch -= dy * 0.2;
	// 	if (camera.pitch < -85) camera.pitch = -85;
	// 	if (camera.pitch > 85) camera.pitch = 85;
	// 	if (camera.yaw < 0) camera.yaw += 360;
	// 	if (camera.yaw > 360) camera.yaw -= 360;
	// }
	// if (mousemiddle || mouseright) {
	// 	camera.distance += dy * 0.01 * camera.distance;
	// 	if (camera.distance < mindist) camera.distance = mindist;
	// 	if (camera.distance > maxdist) camera.distance = maxdist;
	// }
	// mousex = x;
	// mousey = y;
	// glutPostRedisplay();
}

void togglefullscreen(void)
{
	// static int oldw = 100, oldh = 100;
	// static int oldx = 0, oldy = 0;
	// static int isfullscreen = 0;
	// if (!isfullscreen) {
	// 	oldw = screenw;
	// 	oldh = screenh;
	// 	oldx = glutGet(GLUT_WINDOW_X);
	// 	oldy = glutGet(GLUT_WINDOW_Y);
	// 	glutFullScreen();
	// } else {
	// 	glutPositionWindow(oldx, oldy);
	// 	glutReshapeWindow(oldw, oldh);
	// }
	// isfullscreen = !isfullscreen;
}

void stepframe(int dir)
{
	curframe += dir;
	while (curframe < 0) curframe += curanim->len;
	while (curframe >= curanim->len) curframe -= curanim->len;
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27: case 'q': exit(1); break;
	case 'h': case '?': showhelp = !showhelp; break;
	case 'f': togglefullscreen(); break;
	case 'i': doperspective = 0; camera.yaw = 45; camera.pitch = -DIMETRIC; break;
	case 'I': doperspective = 0; camera.yaw = 45; camera.pitch = -ISOMETRIC; break;
	case 'D': doperspective = 0; camera.yaw = 45; camera.pitch = -DIABLO; break;
	case 'p': doperspective = !doperspective; break;
	case 'g': doplane = !doplane; break;
	case 't': dotexture = !dotexture; break;
	case 'w': dowire = !dowire; break;
	case 'b': dobackface = !dobackface; break;
	case 's': doskeleton = !doskeleton; break;
	case 'j': doJointNames = !doJointNames; break;
	case ' ': doplay = !doplay; break;
	case '0': curframe = 0; if (pDrawModel && curanim) animatemodel(pDrawModel, curanim, curframe); break;
	case ',': if (pDrawModel && curanim) { stepframe(-1); animatemodel(pDrawModel, curanim, curframe); } break;
	case '.': if (pDrawModel && curanim) { stepframe(1); animatemodel(pDrawModel, curanim, curframe); } break;
	case '<':
		if (pDrawModel && curanim && curanim->prev) {
			curanim = curanim->prev;
			curframe = 0;
			animatemodel(pDrawModel, curanim, curframe);
		}
		break;
	case '>':
		if (pDrawModel && curanim && curanim->next) {
			curanim = curanim->next;
			curframe = 0;
			animatemodel(pDrawModel, curanim, curframe);
		}
		break;
	}

	// if (pDrawModel && doplay) {
	// 	if (!curanim)
	// 		curanim = pDrawModel->anim;
	// 	lasttime = glutGet(GLUT_ELAPSED_TIME);
	// }

	// glutPostRedisplay();
}

void special(int key, int x, int y)
{
	// switch (key) {
	// case GLUT_KEY_F4: exit(1); break;
	// case GLUT_KEY_F1: showhelp = !showhelp; break;
	// }
	// glutPostRedisplay();
}


static int DrawAoi_tx, DrawAoi_ty, DrawAoi_tw, DrawAoi_th;

void ReshapeForGLUI()
{
//   GLUI_Master.get_viewport_area( &DrawAoi_tx, &DrawAoi_ty, &DrawAoi_tw, &DrawAoi_th );
//   glViewport( DrawAoi_tx, DrawAoi_ty, DrawAoi_tw, DrawAoi_th );

//   // also copy to this, so projects are OK
// 	screenw = DrawAoi_tw;
// 	screenh = DrawAoi_th;
}

void reshape(int w, int h)
{
#ifdef use_again
	screenw = w;
	screenh = h;
	glViewport(0, 0, w, h);
#else
  ReshapeForGLUI();    // Reshape window size for GLUI, get window position and sizes
#endif
}

void display(void)
{
	char buf[256];
	int i, DoDrawJointNames;

	int thistime = (int)(Timer::ins.getTimePassed() * 1000);
	int timediff = thistime - lasttime;
	lasttime = thistime;

	DoDrawJointNames = false;    // preset no draw of bone names

//   ReshapeForGLUI();    // Reshape window size for GLUI, get window position and sizes

// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// if( pDrawModel == NULL) {           // no model loaded
    // 	goto SkipDrawModel;
	// }

	IqmModel::shader.use();

// 	glEnable(GL_DEPTH_TEST);
// 	glEnable(GL_COLOR_MATERIAL);
// 	glEnable(GL_LIGHTING);

// 	glEnable(GL_LIGHT0);
// 	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

// 	glTranslatef(0, camera.distance, 0);
// 	glRotatef(-camera.pitch, 1, 0, 0);
// 	glRotatef(-camera.yaw, 0, 0, 1);
// #ifndef use_again
// 	glTranslatef(-camera.ObjCenter[0], -camera.ObjCenter[1], -camera.ObjCenter[2]);
// #else
// 	glTranslatef(-camera.ObjSize[0] * 0.5, -camera.ObjSize[1] * 0.5, -camera.ObjSize[2] * 0.5);
// #endif

	IqmModel::shader.setUnifrom(2, glm::mat4(1.0));

	if (doplay && !curanim && pDrawModel->anim) {
    	curanim = pDrawModel->anim;
		// lasttime = glutGet(GLUT_ELAPSED_TIME);
	}

	if (doplay && curanim) {
		// glutPostRedisplay();
		curtime = curtime + (timediff / 1000.0) * curanim->framerate;   // animate with 10.0 frames per second
		curframe = ((int)curtime) % curanim->len;
		animatemodel(pDrawModel, curanim, curframe);
	}

// 	if (dotexture)
// 		glEnable(GL_TEXTURE_2D);
// 	else
// 		glDisable(GL_TEXTURE_2D);

// 	if (dowire)
// 		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
// 	else
// 		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

// 	if (dobackface)
// 		glDisable(GL_CULL_FACE);
// 	else
// 		glEnable(GL_CULL_FACE);

// 	glAlphaFunc(GL_GREATER, 0.2);
// 	glEnable(GL_ALPHA_TEST);
	drawmodel(pDrawModel);
// 	glDisable(GL_ALPHA_TEST);

// 	glDisable(GL_CULL_FACE);
// 	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
// 	glDisable(GL_TEXTURE_2D);

// 	glDisable(GL_LIGHTING);
// 	glDisable(GL_COLOR_MATERIAL);

// 	if (doplane) {
// 		glBegin(GL_LINES);
// 		glColor4f(0.4, 0.4, 0.4, 1);
// 		for (i = -gridsize; i <= gridsize; i ++) {
// 			glVertex3f(i, -gridsize, 0); glVertex3f(i, gridsize, 0);
// 			glVertex3f(-gridsize, i, 0); glVertex3f(gridsize, i, 0);
// 		}
// 		glEnd();
// 	}

// 	glDisable(GL_DEPTH_TEST);

// 	if (doskeleton) {
// 		drawskeleton(pDrawModel);
//   	DoDrawJointNames = doJointNames;    // set no draw of joint names
// 	}

// SkipDrawModel:

// 	glMatrixMode(GL_PROJECTION);
// 	glLoadIdentity();
// 	glOrtho(0, screenw, screenh, 0, -1, 1);

// 	glMatrixMode(GL_MODELVIEW);
// 	glLoadIdentity();

// 	if( pDrawModel && DoDrawJointNames) {

// 	  struct skel *skel = pDrawModel->skel;

//   	glColor4f( 1, 0.5, 0.0, 1);

//   	for (i = 0; i < skel->joint_count; i++) {

//   	  drawstring( skel->TempDrawPosX[ i] + 6, skel->TempDrawPosY[ i] + 4, skel->name[ i]);
//   	}
// 	}

// 	glColor4f( 1, 1, 1, 1);

//   if( pDrawModel != NULL) {

// 	  sprintf(buf, "%d meshes; %d vertices; %d faces; %d joints; %d animations; %d poses",
// 		  pDrawModel->mesh->part_count,
//       pDrawModel->mesh->vertex_count,
//       pDrawModel->mesh->element_count/3,
//       pDrawModel->skel->joint_count,
//       pDrawModel->anim_count,
//       pDrawModel->anim_poses);
// 	  drawstring(8, 18+0, buf);

// 	  sprintf( buf, "Model sizes: %.2f / %.2f / %.2f",
//       camera.ObjSize[ 0], camera.ObjSize[ 1], camera.ObjSize[ 2]);
// 	  drawstring(8, 18+16, buf);

// 	  if (curanim) {
// 		  sprintf(buf, "%s (%03d / %03d), %.1f fps",
//         curanim->name, curframe + 1, curanim->len, curanim->framerate);
// 		  drawstring(8, 18+32, buf);
// 	  }
//   } else {

// 	  sprintf(buf, "No model loaded");
// 	  drawstring(8, 18+0, buf);
//   }

//   // Help

// 	if (showhelp) {
// 		#define Y(n) 18+56+n*14
// 		glColor4f(1, 1, 0.5, 1);
// 		drawstring(8, Y(0), (char *)"t - toggle textures");
// 		drawstring(8, Y(1), (char *)"w - toggle wireframe");
// 		drawstring(8, Y(2), (char *)"b - toggle backface culling");
// 		drawstring(8, Y(3), (char *)"s - toggle skeleton");
// 		drawstring(8, Y(4), (char *)"j - toggle joint names");
// 		drawstring(8, Y(5), (char *)"g - toggle ground plane");
// 		drawstring(8, Y(7), (char *)"p - toggle orthogonal/perspective camera");
// 		drawstring(8, Y(8), (char *)"i - set up dimetric camera (2:1)");
// 		drawstring(8, Y(9), (char *)"I - set up isometric camera (true)");
// 		drawstring(8, Y(11), (char *)"space - start/stop animation");
// 		drawstring(8, Y(12), (char *)"',' and '.' - step animation frame by frame");
// 		drawstring(8, Y(13), (char *)"'<' and '>' - switch animation");
// 	}

// 	// draw message

// 	if( DrawMessageText1[ 0] != '\0') {   // have to draw message

//     glColor4f( DrawMessageColR, DrawMessageColG, DrawMessageColB, 1);

// 		drawstring( 8, DrawAoi_th - 42, DrawMessageText1);
// 		drawstring( 8, DrawAoi_th - 26, DrawMessageText2);
// 		drawstring( 8, DrawAoi_th - 10, DrawMessageText3);
// 	}

// 	// Program Version

//   glColor4f( 1, 0.0, 0.0, 1);
//   drawstring( DrawAoi_tw - (strlen( WIN_PROG_VERSION) + 1) * 8, DrawAoi_th - 10, (char *)WIN_PROG_VERSION);

// 	glutSwapBuffers();

// 	i = glGetError();
// 	if (i) fprintf(stderr, "opengl error: %d\n", i);
}

/************************************************************************************
 * PrepareNewFileSelectionModel
 */

void PrepareNewFileSelectionModel( char *pFilename)
{
  char *pFileType;
  char *p;

  // clean things

  freemodel( &pFileSelModel);      // unload allocated data

  FileSelName[0] = '\0';
  FileSelFullName[0] = '\0';

  curanim = NULL;

  // try to load file

  if( pFilename != NULL) {

    pFileType = strrchr( pFilename, '.');        // get pointer to file type

    if( !strcasecmp( pFileType, ".iqe")) {       // is .iqe file

  		pFileSelModel = IQE_loadmodel( pFilename);

    } else if( !strcasecmp( pFileType, ".iqm")) { // is .iqm file

  		pFileSelModel = IQM_loadmodel( pFilename);

    } else {                                     // file type not know

      pFileSelModel = NULL;
    }

		if( pFileSelModel != NULL) {        // load was OK

		  float radius = measuremodel( pFileSelModel, camera.ObjCenter, camera.ObjSize);
		  camera.distance = radius * 2.5;
		  gridsize = (int)radius + 1;
		  mindist = radius * 0.1;
		  maxdist = radius * 10;

		  if (pFileSelModel->mesh->part_count == 0 && pFileSelModel->skel->joint_count > 0)
			  doskeleton = 1;

      // get pointer to file name

	    p = strrchr( pFilename, '/');
	    if (!p) p = strrchr( pFilename, '\\');
	    if (!p) {
        p = pFilename;   // point to begin of file name
      } else {
        p += 1;          // point to begin of file name
      }

      // ...

      strcpy( FileSelName, p);                // file name
      strcpy( FileSelFullName, pFilename);    // path + file name
		}
  }

  SetupDrawModel( pFileSelModel);       // select model to draw
}

/************************************************************************************
 * PrepareContainerModel
 */

void PrepareContainerModel( struct model *pModel, char *pFilename)
{
  char *p;

  // clean things

  freemodel( &pFileSelModel);      // unload allocated data

  FileSelName[0] = '\0';
  FileSelFullName[0] = '\0';

  curanim = NULL;

  // try to load file

  if( pFilename != NULL && pModel != NULL) {

		// glutSetWindowTitle( pFilename);

		if( pModel != NULL) {        // load was OK

		  float radius = measuremodel( pModel, camera.ObjCenter, camera.ObjSize);
		  camera.distance = radius * 2.5;
		  gridsize = (int)radius + 1;
		  mindist = radius * 0.1;
		  maxdist = radius * 10;

		  if (pModel->mesh->part_count == 0 && pModel->skel->joint_count > 0)
			  doskeleton = 1;

      // get pointer to file name

	    p = strrchr( pFilename, '/');
	    if (!p) p = strrchr( pFilename, '\\');
	    if (!p) {
        p = pFilename;   // point to begin of file name
      } else {
        p += 1;          // point to begin of file name
      }

      // ...

      strcpy( FileSelName, p);                // file name
      strcpy( FileSelFullName, pFilename);    // path + file name
		}
  } else {

		// glutSetWindowTitle( WIN_DEFAULT_TITLE);
  }

  SetupDrawModel( pModel);       // select model to draw
}

/************************************************************************************
 * GLUI things
 */

#ifdef __GNUC__
#include <unistd.h>   // needed for getcwd()
#endif

// static GLUI_FileBrowser *pGUI_fb;
// static GLUI_List        *pGUI_Models;
// static GLUI_EditText    *pGUI_WriteFileName;
// static GLUI_Button      *pGUI_ButtonWriteFile;
// static float             GUI_ScaleVale;

// -- update model container

void ModelContainerUpdate( int KeepSelection)
{
//   int i;
//   int LastSelection;

//   LastSelection = pGUI_Models->get_current_item();

//   pGUI_Models->delete_all();       // empty the list box

//   // fill the list box

//   for( i = 0; i < ModelContainer_n; i++) {

//     pGUI_Models->add_item( i, ModelContainer[ i].ModelName);
//   }

//   if( KeepSelection) {

//     pGUI_Models->curr_line = LastSelection;          // set to last added

//     if( pGUI_Models->curr_line >= pGUI_Models->num_lines) {  // security test

//       pGUI_Models->curr_line = pGUI_Models->num_lines - 1;
//     }
//   } else {

//     pGUI_Models->curr_line = ModelContainer_n - 1;   // set to last added
//   }

//   pGUI_Models->start_line = 0;    // display starts at first line

//   if( pGUI_Models->curr_line >= (pGUI_Models->visible_lines - 1)) {   // if current is not vissible

//     pGUI_Models->start_line = pGUI_Models->num_lines - (pGUI_Models->visible_lines - 1);  // try to center

//     if( pGUI_Models->start_line < 0) {    // clip to 0

//       pGUI_Models->start_line = 0;
//     }
//   }

//   pGUI_Models->update_and_draw_text();             // updated

//   if( ModelContainer_n > 0) {     // have any models

//     pGUI_ButtonWriteFile->enable();
//   } else {

//     pGUI_ButtonWriteFile->disable();
//   }

}

// -- GUI control function, called on GUI events

void control_cb( int control) {
//   int i, this_item;
//   char file_name[ 1024];

//   glutPostRedisplay();

//   SetupDrawMessage( DRAW_MSG_COL_OK, NULL, NULL, NULL);  // reset any message

//   switch( control) {

//   case 1:             // list file browser

//     strcpy( file_name, pGUI_fb->current_dir.c_str());

//     if( strcmp( file_name, ".") == 0) {      // is current directory

// #ifdef _WIN32
//       GetCurrentDirectory( sizeof( file_name) - 256, file_name);

// #elif defined(__GNUC__)

//       getcwd( file_name, sizeof( file_name) - 256);
// #endif

//     }

//     strcat( file_name, "/");
//     strcat( file_name, pGUI_fb->get_file());

//     PrepareNewFileSelectionModel( file_name);

//     break;

//   case 2:             // display model container line

//     this_item = pGUI_Models->get_current_item();
//     if (this_item >= 0 && this_item < ModelContainer_n) { // indexes is in range


//       PrepareContainerModel( ModelContainer[ this_item].pModel, ModelContainer[ this_item].ModelFullName);

// 		  curframe = 0;
// 		  if( pDrawModel != NULL) {

// 		    curanim  = pDrawModel->anim;
// 		  } else {

// 		    curanim  = NULL;
// 		  }
//     }

//     break;

//   case 3:             // Add to model container

//     if( pDrawModel == NULL ||                        // have a model loaded
//         pFileSelModel != pDrawModel) {               // must be selected by the file browser

//       SetupDrawMessage( DRAW_MSG_COL_ERR,
//                         (char *)"Add:",
//                         (char *)"Please select a file",
//                         (char *)"with the file browser");

//     } else if( ModelContainer_n >= MODEL_CONTAINER_MAX) {    // container is full

//       SetupDrawMessage( DRAW_MSG_COL_ERR,
//                         (char *)"Add:", (char *)"Model container is full", NULL);

//     } else if( ModelContainer_n < MODEL_CONTAINER_MAX &&    // have free space
//         pDrawModel != NULL &&                        // have a model loaded
//         FileSelName[ 0] != '\0') {                   // have file selected

//       // copy model and names

// #ifdef use_again
//       strcpy( ModelContainerName[ ModelContainer_n], FileSelName);
// #else
//       if( pDrawModel->skel->joint_count == 0 && pDrawModel->anim_poses > 0) { // have NO sekelton but poses from an animation

//         sprintf( ModelContainer[ ModelContainer_n].ModelName, "M%2d,P%2d,A%2d,%s",
//                  pDrawModel->mesh->part_count,
//                  pDrawModel->anim_poses,
//                  pDrawModel->anim_count,
//                  FileSelName);
//       } else {

//         sprintf( ModelContainer[ ModelContainer_n].ModelName, "M%2d,J%2d,A%2d,%s",
//                  pDrawModel->mesh->part_count,
//                  pDrawModel->skel->joint_count,
//                  pDrawModel->anim_count,
//                  FileSelName);
//       }
// #endif

//       // copy the model from the file browser

//       strcpy( ModelContainer[ ModelContainer_n].ModelFullName, FileSelFullName);
//       copymodel( &pDrawModel, &(ModelContainer[ ModelContainer_n].pModel), COPYMODEL_COPY_ALL);

//       // remember the original mesh count

//       ModelContainer[ ModelContainer_n].part_count_before_add = pDrawModel->mesh->part_count;

//       // If this model ist not the first model in the model container,
//       // copy mesh and or sekelton from the first model in the container
//       // if we have only an animation

//       char AddMessage[ 256];

//       strcpy( AddMessage, "");

//       if( ModelContainer_n > 0 &&                    // is NOT the first model
//           ModelContainer[ ModelContainer_n].pModel->anim_count > 0) { // have an animation

//         int CopyFlags;

//         CopyFlags = 0;                               // preset nothing to copy

//         if( ModelContainer[ ModelContainer_n].pModel->skel->joint_count == 0 &&  // have NO skeletion
//             ModelContainer[ 0].pModel->skel->joint_count > 0 &&                  // but first model has one
//             ModelContainer[ ModelContainer_n].pModel->anim_poses ==              // and number of poses of this model
//               ModelContainer[ 0].pModel->skel->joint_count) {                    // match the number of joints of the first model

//           CopyFlags |= COPYMODEL_SKELETON;   // copy the skeleton

//           strcat( AddMessage, "Copied skeleton");
//         }

//         if( ModelContainer[ ModelContainer_n].pModel->mesh->part_count == 0 &&    // have NO meshes
//             ModelContainer[ ModelContainer_n].pModel->mesh->vertex_count == 0 &&  // and have NO vertex data
//             ModelContainer[ 0].pModel->mesh->part_count > 0 &&                    // but first model have a meshes
//             ModelContainer[ 0].pModel->mesh->vertex_count > 0 &&                  // and first model has vertex data
//             ((ModelContainer[ ModelContainer_n].pModel->anim_poses ==             // and number of poses of this model
//                ModelContainer[ 0].pModel->skel->joint_count) ||                   // match the number of joints of the first model
//              (ModelContainer[ ModelContainer_n].pModel->skel->joint_count >= 0 && //   or this model has a skeleton
//                ModelContainer[ 0].pModel->skel->joint_count == 0))) {             //   and the first model has NO skeleton

//           CopyFlags |= COPYMODEL_MESH;       // copy the mesh

//           if( (CopyFlags & COPYMODEL_SKELETON) != 0) {   // also copy skeleton

//             strcat( AddMessage, " and mesh");
//           } else {

//             strcat( AddMessage, "Copied mesh");
//           }
//         }

//         if( CopyFlags != 0) {     // have something to copy

//           copymodel( &(ModelContainer[ 0].pModel), &(ModelContainer[ ModelContainer_n].pModel), CopyFlags);

//           strcat( AddMessage, " from 1. model");
//         }
//       }

//       // ....

//       ModelContainer_n += 1;                         // have one more

//       ModelContainerUpdate( false);                  // update model container

//       // display the just added model

//       PrepareContainerModel( ModelContainer[ ModelContainer_n - 1].pModel, ModelContainer[ ModelContainer_n - 1].ModelFullName);

//       SetupDrawMessage( DRAW_MSG_COL_OK,
//                         (char *)"Add done", AddMessage, NULL);

// 		  curframe = 0;
// 		  if( pDrawModel != NULL) {

// 		    curanim  = pDrawModel->anim;
// 		  } else {

// 		    curanim  = NULL;
// 		  }
//     } else {

//       SetupDrawMessage( DRAW_MSG_COL_ERR,
//                         (char *)"Add failed", NULL, NULL);
//     }

//     break;

//   case 4:             // Remove from model container

//     this_item = pGUI_Models->get_current_item();
//     if (this_item >= 0 && this_item < ModelContainer_n) { // index is in range

//       if( this_item >= ModelContainer_n - 1) {    // is the last one

//         // set last one to nil
//         strcpy( ModelContainer[ ModelContainer_n - 1].ModelName, "");
//         strcpy( ModelContainer[ ModelContainer_n - 1].ModelFullName, "");
//         freemodel( &ModelContainer[ ModelContainer_n - 1].pModel);

//         ModelContainer_n -= 1;                // have one less
//         ModelContainerUpdate( false);         // update model container
//       } else {

//         // copy down

//         freemodel( &ModelContainer[ this_item].pModel);

//         for( i = this_item; i <  ModelContainer_n - 1; i++) {

//           strcpy( ModelContainer[ i].ModelName, ModelContainer[ i + 1].ModelName);
//           strcpy( ModelContainer[ i].ModelFullName, ModelContainer[ i + 1].ModelFullName);
//           ModelContainer[ i].pModel = ModelContainer[ i + 1].pModel;
//         }

//         // set last one to nil
//         strcpy( ModelContainer[ ModelContainer_n - 1].ModelName, "");
//         strcpy( ModelContainer[ ModelContainer_n - 1].ModelFullName, "");
//         ModelContainer[ ModelContainer_n - 1].pModel = NULL;

//         ModelContainer_n -= 1;                // have one less
//         ModelContainerUpdate( true);          // update model container
//       }

//       SetupDrawMessage( DRAW_MSG_COL_OK,
//                         (char *)"Remove done", NULL, NULL);
//     }

//     break;

//   case 5:             // Clear model container

//     if( ModelContainer_n > 0) {       // have something to clear

//       for( i = 0; i <  ModelContainer_n; i++) {

//         strcpy( ModelContainer[ i].ModelName, "");
//         strcpy( ModelContainer[ i].ModelFullName, "");
//         freemodel( &ModelContainer[ i].pModel);
//       }

//       ModelContainer_n = 0;             // set count to 0

//       ModelContainerUpdate( false);     // update model container

//       SetupDrawMessage( DRAW_MSG_COL_OK,
//                       (char *)"Clear done", NULL, NULL);
//    }
//     break;

//   case 6:             // compile model container

//     if( ModelContainer_n > 0 && strlen( pGUI_WriteFileName->get_text()) > 0) {

//       int argc, ResultValue;
//       char *p, *argv[ 1024];
//       char OutputFile[ 1024];
//       char ResultString[ 1024];
//       char ScaleString[ 1024];

//       argc = 0;

//       argv[ argc++] = (char *)"IqmCompiler";       // name of executable

//       // optional scale argument

//       if( GUI_ScaleVale != 1.0) {

//         argv[ argc++] = (char *)"--scale";                // optional scale

//         sprintf( ScaleString, "%f", GUI_ScaleVale);
//         argv[ argc++] = ScaleString;              // argument for scale
//       }

//       // construct output file

//       strcpy( OutputFile, ModelContainer[ 0].ModelFullName);  // get location of first modul
//       p = strrchr( OutputFile, '/');        // try slash
//       if( p == 0) {
//         p = strrchr( OutputFile, '\\');     // try backslash
//       }

//       if( p != 0) {        // got any slash

//         p[ 1] = '\0';      // delete after slash
//         strcat( OutputFile, pGUI_WriteFileName->get_text());  // append output file name
//         strcat( OutputFile,".iqm");                           // append iqm file type
//       } else {

//         strcpy( OutputFile, pGUI_WriteFileName->get_text());  // append output file name
//         strcat( OutputFile,".iqm");                           // append iqm file type
//       }

//       argv[ argc++] = OutputFile;       // name of output

//       // first files arguments are files with meshes, build  comma separeted file list

//       char InputFilesWithMeshes[ 1024 * MODEL_CONTAINER_MAX];

//       strcpy( InputFilesWithMeshes, "");

//       for( i = 0; i < ModelContainer_n; i++) {

//         // test for model with meshes
//         if( ModelContainer[ i].part_count_before_add == 0) {   // have NO meshes

//           break;
//         }

//         // this model has a mesh, add to comma separated file list

//         if( i == 0) {

//           strcpy( InputFilesWithMeshes, ModelContainer[ i].ModelFullName);

//         } else {

//           strcat( InputFilesWithMeshes, ",");
//           strcat( InputFilesWithMeshes, ModelContainer[ i].ModelFullName);
//         }
//       }

//       if( InputFilesWithMeshes[ 0] != '\0') {     // have input files

//         argv[ argc++] = InputFilesWithMeshes;     // input files list
//       }

//       // other files must be animatina files, add to argument list
//       for( ; i < ModelContainer_n; i++) {

//         argv[ argc++] = ModelContainer[ i].ModelFullName;
//       }

//       // call the compiler

//       ResultValue = IQM_Compiler( argc, argv, ResultString);

//       if( ResultValue != 0) {    // have error

//         SetupDrawMessage( DRAW_MSG_COL_ERR,
//                           (char *)"Write IQM failed:", ResultString, NULL);
//       } else {

//         SetupDrawMessage( DRAW_MSG_COL_OK,
//                           (char *)"Write IQM done", NULL, NULL);
//       }
//     }

//     break;

//   default:

//     break;   // used as breakpoint
//   }
}

//-- model list callback

// void Model_list_callback( GLUI_Control *glui_object)
// {
//   int this_item;

//   GLUI_List *list = dynamic_cast<GLUI_List*>(glui_object);
//   if (!list)
//     return;

//   glutPostRedisplay();

//   this_item = list->get_current_item();
//   if (this_item >= 0 && this_item < list->num_lines) {

//     if (this_item >= 0 && this_item < ModelContainer_n) { // indes is in range

// #ifdef use_again
//       PrepareNewModel( ModelContainerFullName[ this_item]);

// 		  curframe = 0;
// 		  curanim  = pDrawModel->anim;
// #else
//       // must do it this way, otherwise loaded textures got lost (wrong window selected or so).
//       list->execute_callback();
// #endif
//     }
//   }
// }

//-- Setup the GUI things

// void GLUI_Setup( GLUI *ep)
// {
//   GLUI_Panel  *pGUI_GroupFrame1, *pGUI_GroupFrame2;
//   GLUI_Button *pGUI_Button;
//   GLUI_EditText *pGUI_EditText;

//   // File browser

//   pGUI_fb = new GLUI_FileBrowser(ep, "File browser", GLUI_PANEL_EMBOSSED, 1, control_cb, (char *)".iqe;.iqm");
//   pGUI_fb->set_w( 254);
//   pGUI_fb->set_h( GLUI_DEFAULT_CONTROL_HEIGHT * 24);
//   pGUI_fb->list->set_click_type(GLUI_SINGLE_CLICK);

//   // Model container

//   pGUI_GroupFrame1 = new GLUI_Panel( ep, "Model container", GLUI_PANEL_EMBOSSED);
//   pGUI_Models = new GLUI_List( pGUI_GroupFrame1, true, 2, control_cb);
//   pGUI_Models->set_w( 250);
//   pGUI_Models->set_h( 15 * MODEL_CONTAINER_DISP + 20);
//   pGUI_Models->set_click_type(GLUI_SINGLE_CLICK);
//   pGUI_Models->set_object_callback( Model_list_callback);

//   // buttons for model container

//   pGUI_GroupFrame2 = new GLUI_Panel( pGUI_GroupFrame1, "", GLUI_PANEL_NONE);
//   pGUI_GroupFrame2->set_alignment( GLUI_ALIGN_LEFT);

//   pGUI_Button = ep->add_button_to_panel( pGUI_GroupFrame2, "Add", 3, control_cb);
//   pGUI_Button->set_w( 70);

//   ep->add_column_to_panel( pGUI_GroupFrame2, false);

//   pGUI_Button = ep->add_button_to_panel( pGUI_GroupFrame2, "Remove", 4, control_cb);
//   pGUI_Button->set_w( 70);

//   ep->add_column_to_panel( pGUI_GroupFrame2, false);

//   pGUI_Button = ep->add_button_to_panel( pGUI_GroupFrame2, "Clear", 5, control_cb);
//   pGUI_Button->set_w( 70);

//   // write file

//   pGUI_GroupFrame2 = new GLUI_Panel( pGUI_GroupFrame1, "", GLUI_PANEL_NONE);
//   pGUI_GroupFrame2->set_alignment( GLUI_ALIGN_LEFT);

//   pGUI_ButtonWriteFile = ep->add_button_to_panel( pGUI_GroupFrame2, "Write IQM", 6, control_cb);
//   pGUI_ButtonWriteFile->set_w( 70);
//   pGUI_ButtonWriteFile->disable();    // disable write file button

//   ep->add_column_to_panel( pGUI_GroupFrame2, false);

//   pGUI_WriteFileName = new GLUI_EditText( pGUI_GroupFrame2, "", GLUI_EDITTEXT_TEXT, -1, control_cb);
//   pGUI_WriteFileName->text_x_offset = 0;
//   pGUI_WriteFileName->set_w( 180);
//   pGUI_WriteFileName->set_text( "MyModel");

//   GUI_ScaleVale = 1.0;
//   pGUI_EditText = ep->add_edittext_to_panel( pGUI_GroupFrame2, "Scale",
//                              GLUI_EDITTEXT_FLOAT, &GUI_ScaleVale, -1, GLUI_CB());
//   pGUI_EditText->text_x_offset = 0;

// }

/************************************************************************************
 * main
 */

int notmain(int argc, char **argv)
{
//   int main_win;
// 	float clearcolor[4] = { 0.22, 0.22, 0.22, 1.0 };

// 	Swap_Init();                              // setup byte swap things

// 	glutInitWindowPosition(50, 50+24);
// 	glutInitWindowSize(screenw, screenh);
// 	glutInit(&argc, argv);
// 	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

// 	main_win = glutCreateWindow("IQE Viewer");
// 	screenw = glutGet(GLUT_WINDOW_WIDTH);
// 	screenh = glutGet(GLUT_WINDOW_HEIGHT);

// #ifdef __APPLE__
// 	int one = 1;
// 	void *ctx = CGLGetCurrentContext();
// 	CGLSetParameter(ctx, kCGLCPSwapInterval, &one);
// #endif

// 	initchecker();

// 	if (argc > 1) {

//     PrepareNewFileSelectionModel( argv[1]);
//   } else {

//     PrepareNewFileSelectionModel( NULL);
// 	}

// 	glEnable(GL_MULTISAMPLE);
// 	glEnable(GL_NORMALIZE);
// 	glDepthFunc(GL_LEQUAL);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
// 	glClearColor(clearcolor[0], clearcolor[1], clearcolor[2], clearcolor[3]);

// #ifdef use_again
// 	glutReshapeFunc(reshape);
// 	glutDisplayFunc(display);
// 	glutMouseFunc(mouse);
// 	glutMotionFunc(motion);
// 	glutKeyboardFunc(keyboard);
// 	glutSpecialFunc(special);
// #else

//   GLUI_Master.set_glutReshapeFunc( reshape);
//   GLUI_Master.set_glutKeyboardFunc( keyboard);
//   GLUI_Master.set_glutSpecialFunc( special);
//   GLUI_Master.set_glutMouseFunc( mouse);
//   GLUI_Master.set_glutIdleFunc( NULL);

// 	glutDisplayFunc(display);
// 	glutMotionFunc(motion);

//   GLUI *glui_subwin = GLUI_Master.create_glui_subwindow( main_win, GLUI_SUBWINDOW_LEFT);
//   glui_subwin->set_main_gfx_window( main_win);

//   GLUI_Setup( glui_subwin);
// #endif

// 	glutMainLoop();

	return 0;
}

/**************************** End Of File *****************************/
