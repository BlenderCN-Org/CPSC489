#include<iostream>
using namespace std;

typedef unsigned int uint32;
typedef float real32;

int main()
{
 uint32 n_markers = 6;
 real32 markers[6] = { 0.0f, 5.0f, 8.0f, 8.0f, 8.0f, 9.0f };

 uint32 curr = 1; // marker index
 uint32 next = 2; // marker index
 real32 base = 5.0f; // time
 real32 time = 7.0f; // time

 real32 dt = 0.5f;
 for(uint32 i = 0; i < 5; i++)
    {
     // time values
     real32 curr_time = time + dt;
     real32 last_time = markers[n_markers - 1];
     if(curr_time > last_time) curr_time = last_time;

     // interval times
     real32 AT = markers[curr];
     real32 BT = markers[next];
     real32 MT = curr_time;
     cout << "BEFORE SHIFTING" << endl;
     cout << " curr = " << curr << endl;
     cout << " next = " << next << endl;
     cout << " AT = " << AT << endl;
     cout << " BT = " << BT << endl;
     cout << " MT = " << MT << endl;

     // must shift
     while(!(MT >= AT && MT < BT)) {
           if(next == n_markers - 1) break;
           curr++;
           next++;
           cout << "SHIFTING" << endl;
           AT = markers[curr];
           BT = markers[next];
          }

     cout << "AFTER SHIFTING" << endl;
     cout << " curr = " << curr << endl;
     cout << " next = " << next << endl;
     cout << " AT = " << AT << endl;
     cout << " BT = " << BT << endl;
     cout << " MT = " << MT << endl;

     if(MT == AT)
       {
        cout << "L-side" << endl;
       }
     else if(MT == BT)
       {
        cout << "R-side" << endl;
       }
     else
       {
        cout << "BETWEEN" << endl;
       }

     // update time
     time = curr_time;
     cout << "time = " << time << endl;
     cout << endl;
    }

 return -1;
}