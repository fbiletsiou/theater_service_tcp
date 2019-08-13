//init values
#define N_seat 280
#define N_tel 9
#define N_seat_min 1
#define N_seat_max 6
#define T_seat_min 4
#define T_seat_max 11
#define T_ihitiko 10
#define P_card_success 85 // %
#define C_seat 20

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
