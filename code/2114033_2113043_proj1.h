//init values
#define N_tel 8
#define N_cash 4

#define N_seat 10

#define N_zoneA 5
#define N_zoneB 10
#define N_zoneC 10

#define P_zoneA 20 // %
#define P_zoneB 40 // %
#define P_zoneC 40 // %

#define C_zoneA 30
#define C_zoneB 25
#define C_zonec 20


#define N_seat_min 1
#define N_seat_max 5

#define T_seat_min 5
#define T_seat_max 10

#define T_cash_min 2
#define T_cash_max 4

#define P_card_success 90 // %

int balance = 0;
int N_transactions = 0;
int N_cust;

int available_teleph = N_tel;

//theater plan
struct seats
{
    int seat_plan [10][28];
    int available_seats;
    int occupied_seats;

}theater_plan;
