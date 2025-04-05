#!/bin/bash

# Định nghĩa các màu sắc cho giao diện
RED='\033[0;31m'      # Màu đỏ cho thông báo lỗi
GREEN='\033[0;32m'    # Màu xanh lá cho thông báo thành công
YELLOW='\033[1;33m'   # Màu vàng cho tiêu đề
BLUE='\033[0;34m'     # Màu xanh dương cho menu
NC='\033[0m'          # Reset màu
BOLD='\033[1m'        # Chữ in đậm

# Hàm hiển thị tiêu đề chương trình
display_header() {
    clear
    echo -e "${BLUE}${BOLD}╔════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}${BOLD}║      QUẢN LÝ CÀI ĐẶT PHẦN MỀM          ║${NC}"
    echo -e "${BLUE}${BOLD}╚════════════════════════════════════════╝${NC}"
    echo
}

# Hàm kiểm tra quyền root
check_root() {
    if [ "$(id -u)" != "0" ]; then
        echo -e "${RED}Lỗi: Script này cần được chạy với quyền root${NC}"
        echo "Vui lòng sử dụng: sudo $0"
        exit 1
    fi
}

# Hàm cài đặt phần mềm
install_program() {
    echo -e "${YELLOW}${BOLD}Cài đặt phần mềm mới${NC}"
    echo "----------------------------------------"
    echo -e "${BLUE}${BOLD}Hướng dẫn:${NC}"
    echo "- Nhập tên chính xác của gói phần mềm"
    echo "- Ví dụ: firefox, vlc, gimp, etc."
    echo "----------------------------------------"
    
    read -p "Nhập tên phần mềm cần cài đặt: " program_name
    
    if [ -z "$program_name" ]; then
        echo -e "${RED}Lỗi: Tên phần mềm không được để trống${NC}"
        return
    fi

    echo -e "${YELLOW}Đang cập nhật danh sách gói...${NC}"
    apt-get update > /dev/null 2>&1

    echo -e "${YELLOW}Đang cài đặt $program_name...${NC}"
    if apt-get install -y "$program_name"; then
        echo -e "${GREEN}✓ Đã cài đặt $program_name thành công${NC}"
    else
        echo -e "${RED}Lỗi: Không thể cài đặt $program_name${NC}"
    fi
    
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Hàm gỡ bỏ phần mềm
remove_program() {
    echo -e "${YELLOW}${BOLD}Gỡ bỏ phần mềm${NC}"
    echo "----------------------------------------"
    echo -e "${BLUE}${BOLD}Hướng dẫn:${NC}"
    echo "- Nhập tên chính xác của gói phần mềm cần gỡ bỏ"
    echo "- Thao tác này sẽ xóa phần mềm khỏi hệ thống"
    echo "----------------------------------------"
    
    read -p "Nhập tên phần mềm cần gỡ bỏ: " program_name
    
    if [ -z "$program_name" ]; then
        echo -e "${RED}Lỗi: Tên phần mềm không được để trống${NC}"
        return
    fi

    echo -e "${RED}Cảnh báo: Bạn có chắc muốn gỡ bỏ $program_name? (y/n)${NC}"
    read -p "Xác nhận: " confirm
    
    if [ "$confirm" = "y" ]; then
        echo -e "${YELLOW}Đang gỡ bỏ $program_name...${NC}"
        if apt-get remove -y "$program_name"; then
            echo -e "${GREEN}✓ Đã gỡ bỏ $program_name thành công${NC}"
        else
            echo -e "${RED}Lỗi: Không thể gỡ bỏ $program_name${NC}"
        fi
    else
        echo -e "${YELLOW}Đã hủy thao tác gỡ bỏ${NC}"
    fi
    
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Hàm tìm kiếm phần mềm
search_program() {
    echo -e "${YELLOW}${BOLD}Tìm kiếm phần mềm${NC}"
    echo "----------------------------------------"
    read -p "Nhập tên phần mềm cần tìm: " search_term
    
    if [ -z "$search_term" ]; then
        echo -e "${RED}Lỗi: Từ khóa tìm kiếm không được để trống${NC}"
        return
    fi

    echo -e "${YELLOW}Đang tìm kiếm $search_term...${NC}"
    apt-cache search "$search_term"
    
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Kiểm tra quyền root
check_root

# Menu chính của chương trình
while true; do
    display_header
    echo -e "${BLUE}${BOLD}MENU CHỨC NĂNG:${NC}"
    echo -e "1. Cài đặt phần mềm mới"
    echo -e "2. Gỡ bỏ phần mềm"
    echo -e "3. Tìm kiếm phần mềm"
    echo -e "0. Thoát"
    echo
    read -p "Nhập lựa chọn của bạn: " choice

    case $choice in
        1) install_program ;;
        2) remove_program ;;
        3) search_program ;;
        0)
            echo -e "${GREEN}${BOLD}Cảm ơn bạn đã sử dụng chương trình!${NC}"
            exit 0
            ;;
        *)
            echo -e "${RED}Lựa chọn không hợp lệ${NC}"
            sleep 2
            ;;
    esac
done

