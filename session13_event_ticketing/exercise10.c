#include <stdio.h>
#include <string.h>

#define MAX_ZONES 10
#define MAX_QRS 50
#define MAX_PER_ORDER 4

struct TicketZone {
    int zone_id;
    char zone_name[30];
    float ticket_price;
    int total_capacity;
    int sold_tickets;
};


struct QrCheckIn {
    int qr_code;
    int zone_id;
    int is_used; 
};

int timKhuVuc(const struct TicketZone zones[], int total_zones, int zone_id);
int timMaQr(const struct QrCheckIn qrs[], int total_qrs, int qr_code);
int muaVe(struct TicketZone zones[], int total_zones, struct QrCheckIn qrs[], int *total_qrs, int zone_id, int quantity, int is_early_bird);
int checkInQr(struct QrCheckIn qrs[], int total_qrs, int qr_code);
void hienThiTrangThai(const struct TicketZone zones[], int total_zones, const struct QrCheckIn qrs[], int total_qrs);

int main() {
    struct TicketZone zones[MAX_ZONES];
    int total_zones = 3;

    struct QrCheckIn qrs[MAX_QRS];
    int total_qrs = 0;

    zones[0] = (struct TicketZone){1, "VIP", 2500000.0f, 5, 0};
    zones[1] = (struct TicketZone){2, "Zone A", 1500000.0f, 10, 8}; // Con lai 2 ve de test overbooking
    zones[2] = (struct TicketZone){3, "GA", 800000.0f, 20, 0};

    printf("=== HE THONG QUAN LY BAN VE & CHECK-IN TICKETBOX LIVE CONCERT ===\n\n");

    printf("[TEST 1]: Khach mua 2 ve VIP (Early Bird)...\n");
    muaVe(zones, total_zones, qrs, &total_qrs, 1, 2, 1);

    printf("\n[TEST 2]: Khach mua 5 ve Zone A (Vi pham han muc)...\n");
    muaVe(zones, total_zones, qrs, &total_qrs, 2, 5, 0);

    printf("\n[TEST 3]: Khach dat 3 ve Zone A (Vuot suc chua con lai)...\n");
    muaVe(zones, total_zones, qrs, &total_qrs, 2, 3, 0);

    printf("\n[TEST 4]: Mua ve voi ma khu vuc khong ton tai (99)...\n");
    muaVe(zones, total_zones, qrs, &total_qrs, 99, 2, 0);

    printf("\n[TEST PHU]: Mua 1 ve GA de lay ma QR test...\n");
    muaVe(zones, total_zones, qrs, &total_qrs, 3, 1, 1);

    int test_qr = qrs[0].qr_code;
    printf("\n[TEST 5]: Quet check-in lan dau cho ma QR: %d...\n", test_qr);
    checkInQr(qrs, total_qrs, test_qr);

    printf("\n[TEST 6]: Quet check-in lai lan 2 cho ma QR: %d (Kiem tra trung lap)...\n", test_qr);
    checkInQr(qrs, total_qrs, test_qr);

    printf("\n[TEST 7]: Check-in voi ma QR khong ton tai (8888)...\n");
    checkInQr(qrs, total_qrs, 8888);

    printf("\n>>> TONG KET TRANG THAI HE THONG SAU KHI GIAO DICH <<<\n");
    hienThiTrangThai(zones, total_zones, qrs, total_qrs);

    return 0;
}

int timKhuVuc(const struct TicketZone zones[], int total_zones, int zone_id) {
    for (int i = 0; i < total_zones; i++) {
        if (zones[i].zone_id == zone_id) {
            return i;
        }
    }
    return -1;
}

int timMaQr(const struct QrCheckIn qrs[], int total_qrs, int qr_code) {
    for (int i = 0; i < total_qrs; i++) {
        if (qrs[i].qr_code == qr_code) {
            return i;
        }
    }
    return -1;
}

int muaVe(struct TicketZone zones[], int total_zones, struct QrCheckIn qrs[], int *total_qrs, int zone_id, int quantity, int is_early_bird) {
    if (quantity < 1 || quantity > MAX_PER_ORDER) {
        printf("[LOI]: So luong mua khong hop le (%d). Chi cho phep tu 1 den %d ve/don!\n", quantity, MAX_PER_ORDER);
        return -1;
    }

    int idx = timKhuVuc(zones, total_zones, zone_id);
    if (idx == -1) {
        printf("[LOI]: Ma khu vuc %d khong ton tai trong he thong!\n", zone_id);
        return -2;
    }

    if (zones[idx].sold_tickets + quantity > zones[idx].total_capacity) {
        printf("[LOI]: Khu vuc '%s' khong du cho trong! Chi con lai %d ve.\n", 
               zones[idx].zone_name, zones[idx].total_capacity - zones[idx].sold_tickets);
        return -3;
    }

    float giaThanhToan = zones[idx].ticket_price;
    if (is_early_bird == 1) {
        giaThanhToan = giaThanhToan * 0.85f;
    }
    float tongTien = giaThanhToan * quantity;

    zones[idx].sold_tickets += quantity;

    for (int i = 0; i < quantity; i++) {
        if (*total_qrs >= MAX_QRS) {
            printf("[CANH BAO]: Danh sach QR da day, khong the cap phat them ma QR!\n");
            break;
        }
        struct QrCheckIn new_qr;
        new_qr.qr_code = 5000 + (*total_qrs) + 1;
        new_qr.zone_id = zone_id;
        new_qr.is_used = 0;

        qrs[*total_qrs] = new_qr;
        (*total_qrs)++;
    }

    printf("[THANH CONG]: Dat mua thanh cong %d ve khu vuc '%s'. Tong tien: %.0f VND (Early Bird: %s). Da phat hanh ma QR.\n",
           quantity, zones[idx].zone_name, tongTien, is_early_bird ? "Co" : "Khong");
    return 1;
}

int checkInQr(struct QrCheckIn qrs[], int total_qrs, int qr_code) {
    int idx = timMaQr(qrs, total_qrs, qr_code);
    if (idx == -1) {
        printf("[LOI]: Ma QR %d khong ton tai trong he thong!\n", qr_code);
        return -1;
    }

    if (qrs[idx].is_used == 1) {
        printf("[CANH BAO GIAN LAN]: Ma QR %d da duoc su dung truoc do! Tu choi vao cong.\n", qr_code);
        return 0;
    }

    qrs[idx].is_used = 1;
    printf("[CHECK-IN THANH CONG]: Xac thuc thanh cong ma QR %d. Chao mung khan gia vao su kien!\n", qr_code);
    return 1;
}

void hienThiTrangThai(const struct TicketZone zones[], int total_zones, const struct QrCheckIn qrs[], int total_qrs) {
    printf("\n--- DANH SACH CAC KHU VUC VE ---\n");
    for (int i = 0; i < total_zones; i++) {
        printf("Ma KV: %d | Ten KV: %s | Gia goc: %.0f | Suc chua: %d | Da ban: %d | Con lai: %d\n",
               zones[i].zone_id,
               zones[i].zone_name,
               zones[i].ticket_price,
               zones[i].total_capacity,
               zones[i].sold_tickets,
               zones[i].total_capacity - zones[i].sold_tickets);
    }

    printf("\n--- DANH SACH MA QR CHECK-IN (%d ma) ---\n", total_qrs);
    for (int i = 0; i < total_qrs; i++) {
        printf("STT: %d | Ma QR: %d | Ma KV: %s | Trang thai: %s\n",
               i + 1,
               qrs[i].qr_code,
               qrs[i].zone_id == 1 ? "VIP" : (qrs[i].zone_id == 2 ? "Zone A" : "GA"),
               qrs[i].is_used ? "Da su dung (1)" : "Chua su dung (0)");
    }
}