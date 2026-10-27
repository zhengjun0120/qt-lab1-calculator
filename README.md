# 带键盘事件的计算器

Qt 6 Widgets 写的四则运算计算器，鼠标和键盘共用同一套处理函数。

## 环境

- Qt 6.11.2（MinGW 13.1.0 64 位）
- CMake 3.30，Ninja
- 开发工具 Qt Creator

## 构建

注意：moc 在含中文的目录下无法生成文件，因此构建目录必须放在纯英文路径。

命令行构建：

```bat
cmake -S Calculator -B D:/homework/qt1/build-calculator -G Ninja ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_PREFIX_PATH=D:/Qt/6.11.2/mingw_64 ^
      -DCMAKE_CXX_COMPILER=D:/Qt/Tools/mingw1310_64/bin/g++.exe

cmake --build D:/homework/qt1/build-calculator
```

用 Qt Creator 打开 `Calculator/CMakeLists.txt` 时，需要把构建设置里的 Shadow build 目录改成纯英文路径，例如 `D:/homework/qt1/build-calculator`，否则 moc 会报 `Cannot create ... No such file or directory`。

## 按键对照

| 按键 | 功能 |
| --- | --- |
| 0-9 | 输入数字 |
| . | 输入小数点 |
| + - * / | 加减乘除 |
| Enter / = | 计算 |
| Backspace | 退格 |
| Esc / Delete | 清除 |

鼠标点击按钮与上述按键走同一套处理函数。
