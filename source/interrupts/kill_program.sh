#!/bin/bash

# Tìm PID của chương trình
pid=$(pgrep -x "$1")

if [ -z "$pid" ]; then
    echo "Không tìm thấy chương trình: $1"
    exit 1
fi

# Hiển thị thông tin về tiến trình
echo "Đã tìm thấy tiến trình:"
ps -p $pid -o pid,ppid,cmd

# Gửi PID đến module
echo "Đang cấu hình module để kết thúc PID: $pid"
echo $pid | sudo tee /proc/hotkey_process_terminator

echo "Đã cấu hình xong! Nhấn Ctrl+Alt+K để kết thúc tiến trình."
