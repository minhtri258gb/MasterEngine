#define __MT_TEST_CPP__

#include "common.h"
#include "Test.h"
#include "file/TestJson.h"
#include "audio/Audio.h"
#include "technique/TestTechnique.h"
#include "thread/Thread.h"

using namespace std;
using namespace mt::test;

void Test::run()
{
	// TestJson test; // faild hien loi nhung build duoc
	// TestTechnique test;
	// Thread test;

	// test.run();

	// ===============================================================================================

	// vec3 a(1,1,1);
	// vec3 b(10,10,10);
	// vec3 c = a.mix(b, 0.5f);
	// cout << a.x << ' ' << a.y << ' ' << a.z << endl;
	// cout << b.x << ' ' << b.y << ' ' << b.z << endl;
	// cout << c.x << ' ' << c.y << ' ' << c.z << endl;

	// ===============================================================================================
	// float width = 1920.0f;
	// float height = 1080.0f;

	// glm::mat4 ma = glm::mat4(1.0f);
	// // glm::mat4 ma = glm::mat4(5,0,0,0,0,0,3,0,0,1,3,0,1,0,0,1);
	// ma = glm::ortho(0.0f, (float)width, 0.0f, (float)height, 0.1f, 100.0f);

	// cout << ma[0][0] << '\t' << ma[0][1] << '\t' << ma[0][2] << '\t' << ma[0][3] << '\t' << endl;
	// cout << ma[1][0] << '\t' << ma[1][1] << '\t' << ma[1][2] << '\t' << ma[1][3] << '\t' << endl;
	// cout << ma[2][0] << '\t' << ma[2][1] << '\t' << ma[2][2] << '\t' << ma[2][3] << '\t' << endl;
	// cout << ma[3][0] << '\t' << ma[3][1] << '\t' << ma[3][2] << '\t' << ma[3][3] << '\t' << endl;
	// cout << "============================\n";

	// // mat4 mb(5,0,0,0,0,0,3,0,0,1,3,0,1,0,0,1);
	// mat4 mb;
	// mb.ortho(0.0f, (float)width, 0.0f, (float)height, 0.1f, 100.0f);

	// cout << mb[ 0] << '\t' << mb[ 1] << '\t' << mb[ 2] << '\t' << mb[ 3] << '\t' << endl;
	// cout << mb[ 4] << '\t' << mb[ 5] << '\t' << mb[ 6] << '\t' << mb[ 7] << '\t' << endl;
	// cout << mb[ 8] << '\t' << mb[ 9] << '\t' << mb[10] << '\t' << mb[11] << '\t' << endl;
	// cout << mb[12] << '\t' << mb[13] << '\t' << mb[14] << '\t' << mb[15] << '\t' << endl;

}
