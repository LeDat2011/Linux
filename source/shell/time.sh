#!/bin/bash

# Định nghĩa các màu sắc cho giao diện
RED='\033[0;31m'      # Màu đỏ cho thông báo lỗi
GREEN='\033[0;32m'    # Màu xanh lá cho thông báo thành công
YELLOW='\033[1;33m'   # Màu vàng cho tiêu đề
BLUE='\033[0;34m'     # Màu xanh dương cho menu
NC='\033[0m'          # Reset màu
BOLD='\033[1m'        # Chữ in đậm

# Hàm kiểm tra quyền root
check_root() {
    if [ "$(id -u)" != "0" ]; then
        echo -e "${RED}Lỗi: Script này cần được chạy với quyền root${NC}"
        echo "Vui lòng sử dụng: sudo $0"
        exit 1
    fi
}

# Hàm hiển thị tiêu đề chương trình
display_header() {
    clear
    echo -e "${BLUE}${BOLD}╔════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}${BOLD}║        QUẢN LÝ THỜI GIAN HỆ THỐNG      ║${NC}"
    echo -e "${BLUE}${BOLD}╚════════════════════════════════════════╝${NC}"
    echo
}

# Hàm hiển thị thời gian thực
hien_thi_thoi_gian_thuc() {
    echo -e "${YELLOW}${BOLD}Thời gian hiện tại:${NC}"
    echo "----------------------------------------"
    echo -e "${GREEN}$(date +"%A, %d %B %Y %T %Z")${NC}"
    echo "----------------------------------------"
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Hàm cài đặt thời gian hệ thống
cai_dat_thoi_gian_he_thong() {
    echo -e "${YELLOW}${BOLD}Cài đặt thời gian hệ thống${NC}"
    echo "----------------------------------------"
    echo -e "Thời gian hiện tại: ${GREEN}$(date)${NC}"
    echo "----------------------------------------"
    
    # Hiển thị hướng dẫn chi tiết
    echo -e "\n${BLUE}${BOLD}Hướng dẫn nhập thời gian:${NC}"
    echo "- Định dạng: YYYY-MM-DD HH:MM:SS"
    echo "- YYYY: Năm (4 chữ số)"
    echo "- MM: Tháng (01-12)"
    echo "- DD: Ngày (01-31)"
    echo "- HH: Giờ (00-23)"
    echo "- MM: Phút (00-59)"
    echo "- SS: Giây (00-59)"
    echo -e "${YELLOW}Ví dụ: 2024-02-20 15:30:00${NC}"
    
    # Tạm thời tắt dịch vụ systemd-timesyncd
    if systemctl stop systemd-timesyncd; then
        echo -e "${GREEN}Đã tạm dừng dịch vụ đồng bộ thời gian${NC}"
    else
        echo -e "${RED}Không thể tạm dừng dịch vụ đồng bộ thời gian${NC}"
        return
    fi

    # Nhập và kiểm tra thời gian mới
    echo -e "\n${BLUE}${BOLD}Nhập thời gian mới:${NC}"
    read -p "Thời gian mới: " thoi_gian_moi

    if ! date -d "$thoi_gian_moi" >/dev/null 2>&1; then
        echo -e "${RED}Lỗi: Định dạng thời gian không hợp lệ${NC}"
        echo -e "${RED}Vui lòng nhập đúng định dạng như ví dụ ở trên${NC}"
        systemctl start systemd-timesyncd
        return
    fi

    # Cập nhật thời gian
    if timedatectl set-time "$thoi_gian_moi" && date -s "$thoi_gian_moi"; then
        echo -e "${GREEN}✓ Đã cập nhật thời gian thành công${NC}"
        echo -e "Thời gian mới: ${GREEN}$(date)${NC}"
    else
        echo -e "${RED}Lỗi: Không thể cập nhật thời gian${NC}"
        systemctl start systemd-timesyncd
    fi
    
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Hàm đồng bộ thời gian hệ thống
dong_bo_thoi_gian_he_thong() {
    echo -e "${YELLOW}${BOLD}Đồng bộ thời gian hệ thống${NC}"
    echo "----------------------------------------"
    echo -e "Thời gian trước khi đồng bộ: ${GREEN}$(date)${NC}"
    
    if systemctl restart systemd-timesyncd; then
        sleep 2  # Chờ đồng bộ
        echo -e "${GREEN}✓ Đã đồng bộ thời gian thành công${NC}"
        echo -e "Thời gian sau khi đồng bộ: ${GREEN}$(date)${NC}"
    else
        echo -e "${RED}Lỗi: Không thể đồng bộ thời gian${NC}"
    fi
    
    echo
    read -p "Nhấn Enter để tiếp tục..."
}

# Kiểm tra quyền root
check_root

# Menu chính của chương trình
while true; do
    display_header
    echo -e "${BLUE}${BOLD}MENU CHỨC NĂNG:${NC}"
    echo -e "1. Hiển thị thời gian thực"
    echo -e "2. Cài đặt thời gian hệ thống"
    echo -e "3. Đồng bộ thời gian hệ thống"
    echo -e "0. Thoát"
    echo
    read -p "Nhập lựa chọn của bạn: " lua_chon

    case $lua_chon in
        1) hien_thi_thoi_gian_thuc ;;
        2) cai_dat_thoi_gian_he_thong ;;
        3) dong_bo_thoi_gian_he_thong ;;
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

