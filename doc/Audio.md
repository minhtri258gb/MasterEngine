
# Phân loại

## Music
Ex:
> Audio::ins.playMusic("file.mp3");

## Sound tempolary
Ex:
> Audio::ins.play("file.wav");

## Sound regularly (cache)
Ex:
> Sound sound = Audio::ins.cacheSound("file.wav");

# Luồng sử dụng
## Cache lại rồi dùng theo sự kiện
Tạo Sound bằng cache rồi play bằng chính sound đó

## Dùng tạm thời
Tạo Sound rỗng, thêm các thuộc tính vào Sound rồi play bằng Audio

# TODO
Tăng giảm âm lượng khi vào combat và ra xem map
