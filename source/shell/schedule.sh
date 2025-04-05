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
    echo -e "${BLUE}${BOLD}║        HỆ THỐNG LẬP LỊCH TỰ ĐỘNG       ║${NC}"
    echo -e "${BLUE}${BOLD}╚════════════════════════════════════════╝${NC}"
    echo
}

# Hàm tạo lịch trình mới
create_schedule() {
    echo -e "${YELLOW}${BOLD}Tạo lịch trình mới${NC}"
    read -p "Nhập đường dẫn đến thư mục: " filepath
    if [ -z "$filepath" ]; then
        echo -e "${RED}Lỗi: Đường dẫn không được để trống${NC}"
        return
    fi
    
    read -p "Nhập giờ thực thi (0-23): " hour
    if ! [[ "$hour" =~ ^[0-9]+$ ]] || [ "$hour" -lt 0 ] || [ "$hour" -gt 23 ]; then
        echo -e "${RED}Lỗi: Giờ không hợp lệ${NC}"
        return
    fi
    
    read -p "Nhập phút thực thi (0-59): " minute
    if ! [[ "$minute" =~ ^[0-9]+$ ]] || [ "$minute" -lt 0 ] || [ "$minute" -gt 59 ]; then
        echo -e "${RED}Lỗi: Phút không hợp lệ${NC}"
        return
    fi

    croncmd="touch $filepath/file_$(date +\%Y\%m\%d_\%H\%M\%S).txt 2>> $filepath/cron.log"
    cronjob="$minute $hour * * * $croncmd"
    
    if (crontab -l 2>/dev/null; echo "$cronjob") | crontab - ; then
        echo -e "${GREEN}✓ Đã tạo lịch trình thành công${NC}"
    else
        echo -e "${RED}Lỗi: Không thể tạo lịch trình${NC}"
    fi
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Hàm xem danh sách lịch trình
view_schedules() {
    echo -e "${YELLOW}${BOLD}Danh sách lịch trình hiện tại${NC}"
    echo "----------------------------------------"
    if crontab -l 2>/dev/null; then
        echo -e "${GREEN}✓ Đã hiển thị tất cả lịch trình${NC}"
    else
        echo -e "${RED}Không có lịch trình nào${NC}"
    fi
    echo "----------------------------------------"
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Hàm xóa lịch trình
delete_schedule() {
    echo -e "${YELLOW}${BOLD}Xóa lịch trình${NC}"
    echo "Danh sách lịch trình hiện tại:"
    echo "----------------------------------------"
    crontab -l
    echo "----------------------------------------"
    read -p "Bạn có muốn xóa tất cả lịch trình không? (y/n): " confirm
    if [ "$confirm" = "y" ]; then
        crontab -r
        echo -e "${GREEN}✓ Đã xóa tất cả lịch trình${NC}"
    else
        echo -e "${YELLOW}Đã hủy thao tác xóa${NC}"
    fi
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Hàm kiểm tra trạng thái cron
check_cron_status() {
    echo -e "${YELLOW}${BOLD}Kiểm tra trạng thái Cron${NC}"
    if systemctl is-active --quiet cron; then
        echo -e "${GREEN}✓ Dịch vụ Cron đang hoạt động${NC}"
    else
        echo -e "${RED}Dịch vụ Cron không hoạt động${NC}"
    fi
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Menu chính của chương trình
while true; do
    display_header
    echo -e "${BLUE}${BOLD}MENU CHỨC NĂNG:${NC}"
    echo -e "1. Tạo lịch trình mới"
    echo -e "2. Xem danh sách lịch trình"
    echo -e "3. Xóa lịch trình"
    echo -e "4. Kiểm tra trạng thái Cron"
    echo -e "0. Thoát"
    echo
    read -p "Nhập lựa chọn của bạn: " choice

    case $choice in
        1) create_schedule ;;
        2) view_schedules ;;
        3) delete_schedule ;;
        4) check_cron_status ;;
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
