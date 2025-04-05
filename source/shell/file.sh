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
    echo -e "${BLUE}${BOLD}║           HỆ THỐNG QUẢN LÝ FILE        ║${NC}"
    echo -e "${BLUE}${BOLD}╚════════════════════════════════════════╝${NC}"
    echo
}

# Hàm hiển thị danh sách file
display_files() {
    echo -e "${YELLOW}${BOLD}Danh sách các file:${NC}"
    ls -l --color=auto
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Hàm tạo file mới
create_file() {
    echo -e "${YELLOW}${BOLD}Tạo file mới${NC}"
    read -p "Nhập tên file mới: " filename
    if [ -z "$filename" ]; then
        echo -e "${RED}Lỗi: Tên file không được để trống${NC}"
        return
    fi
    touch "$filename"
    echo -e "${GREEN}✓ File $filename đã được tạo thành công${NC}"
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Hàm xóa file
delete_file() {
    echo -e "${YELLOW}${BOLD}Xóa file${NC}"
    read -p "Nhập tên file cần xóa: " filename
    if [ ! -f "$filename" ]; then
        echo -e "${RED}Lỗi: File không tồn tại${NC}"
        return
    fi
    rm -i "$filename"
    echo -e "${GREEN}✓ File $filename đã được xóa${NC}"
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Hàm chỉnh sửa file
edit_file() {
    echo -e "${YELLOW}${BOLD}Chỉnh sửa file${NC}"
    read -p "Nhập tên file cần chỉnh sửa: " filename
    if [ ! -f "$filename" ]; then
        echo -e "${RED}Lỗi: File không tồn tại${NC}"
        return
    fi
    nano "$filename"
    echo -e "${GREEN}✓ File $filename đã được chỉnh sửa${NC}"
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Hàm hiển thị nội dung file
display_file_content() {
    echo -e "${YELLOW}${BOLD}Hiển thị nội dung file${NC}"
    read -p "Nhập tên file cần xem nội dung: " filename
    if [ ! -f "$filename" ]; then
        echo -e "${RED}Lỗi: File không tồn tại${NC}"
        return
    fi
    echo -e "${BLUE}${BOLD}Nội dung file $filename:${NC}"
    echo "----------------------------------------"
    cat "$filename"
    echo "----------------------------------------"
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Hàm thay đổi thuộc tính file
change_file_attribute() {
    echo -e "${YELLOW}${BOLD}Thay đổi thuộc tính file${NC}"
    read -p "Nhập tên file cần thay đổi thuộc tính: " filename
    if [ ! -f "$filename" ]; then
        echo -e "${RED}Lỗi: File không tồn tại${NC}"
        return
    fi
    
    while true; do
        echo -e "\n${BLUE}${BOLD}Chọn loại thuộc tính muốn thay đổi:${NC}"
        echo -e "1. Chỉ đọc"
        echo -e "2. Chỉ ghi"
        echo -e "3. File ẩn"
        echo -e "0. Thoát"
        read -p "Lựa chọn của bạn: " attribute_choice

        case $attribute_choice in
            1)
                chmod u-w "$filename"  # Loại bỏ quyền ghi
                echo -e "${GREEN}✓ Đã thay đổi thuộc tính của $filename thành chỉ đọc${NC}"
                ;;
            2)
                chmod u-r "$filename"  # Loại bỏ quyền đọc
                echo -e "${GREEN}✓ Đã thay đổi thuộc tính của $filename thành chỉ ghi${NC}"
                ;;
            3)
                mv "$filename" ".$filename"  # Chuyển thành file ẩn
                echo -e "${GREEN}✓ Đã chuyển $filename thành file ẩn${NC}"
                ;;
            0)
                echo -e "${YELLOW}Thoát chức năng${NC}"
                break
                ;;
            *)
                echo -e "${RED}Lựa chọn không hợp lệ${NC}"
                ;;
        esac
        echo
    done
    read -p "Nhấn Enter để tiếp tục..."
}

# Hàm nén file
compress_file() {
    echo -e "${YELLOW}${BOLD}Nén file${NC}"
    read -p "Nhập tên file hoặc thư mục cần nén: " input_name
    if [ ! -e "$input_name" ]; then
        echo -e "${RED}Lỗi: File hoặc thư mục không tồn tại${NC}"
        return
    fi
    read -p "Nhập tên cho file nén: " output_name
    if [ -f "$input_name" ]; then
        tar -czf "$output_name.tar.gz" "$input_name"
        echo -e "${GREEN}✓ Đã nén $input_name thành $output_name.tar.gz${NC}"
    else
        echo -e "${RED}Lỗi: Không thể nén thư mục${NC}"
    fi
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Menu chính của chương trình
while true; do
    display_header
    echo -e "${BLUE}${BOLD}MENU CHỨC NĂNG:${NC}"
    echo -e "1. Hiển thị danh sách file"
    echo -e "2. Tạo file mới"
    echo -e "3. Xóa file"
    echo -e "4. Chỉnh sửa file"
    echo -e "5. Hiển thị nội dung file"
    echo -e "6. Thay đổi thuộc tính file"
    echo -e "7. Nén file"
    echo -e "0. Thoát"
    echo
    read -p "Nhập lựa chọn của bạn: " choice

    case $choice in
        1) display_files ;;
        2) create_file ;;
        3) delete_file ;;
        4) edit_file ;;
        5) display_file_content ;;
        6) change_file_attribute ;;
        7) compress_file ;;
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

