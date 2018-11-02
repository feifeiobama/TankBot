//
//  main.cpp
//  test
//
//  Created by szc on 2017/5/9.
//  Copyright © 2017年 szc. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <cmath>

using namespace std;

double mutation_rate=0.98;
double mutation_range=0.3;
int step=40;
int play_round=10;

int N = 12;
int last=-10,last1=-10,last2=-10,last0=-10;

struct thread_data{
    double a[20];
};

istream& operator >> (istream &in,thread_data& t){
    for(int i = 0; i < N; ++i)
        in>>t.a[i];
    return in;
};

ostream& operator <<(ostream &out,thread_data& t){
    for(int i = 0; i < N; ++i)
        out<<fixed<<setprecision(6)<<right<<setw(10)<<t.a[i]<<' ';
    return out;
};
void print(thread_data& t){
    for(int i = 0; i < N; ++i)
        cout<<fixed<<setprecision(10)<<right<<setw(14)<<t.a[i]<<' ';
};


string trans_data(thread_data * t){
    string s;
    for(int i = 0; i < N; ++i)
        s = s + " " + to_string(t->a[i]) + " ";
    return s;
}


thread_data mutation(thread_data& t){
    for(int i = 0; i < N; ++i)
        if(i != 7)
            t.a[i] *= pow(2.0,(random()/double(2147483647)-0.5)*mutation_range);
    return t;
}

bool play(thread_data *my_data1,thread_data *my_mutedata,thread_data *my_data0,thread_data *my_data2){
    int sum1=0,sum2=0;
    double sum=0;

    for(int i=0;i<play_round;++i){
        string s="lua bots_c.lua /Users/szc/Desktop/simple_Tetris2/main"+trans_data(my_mutedata)
        +"/Users/szc/Desktop/simple_Tetris2/main"+trans_data(my_data2);
        system(s.c_str());
        
        ifstream temp("temp_judge_out");
        int temp_ans=0;
        temp>>temp_ans;
        sum1+=temp_ans;
        temp.close();
    }
    for(int i=0;i<play_round;++i){
        string s="lua bots_c.lua /Users/szc/Desktop/simple_Tetris2/main"+trans_data(my_mutedata)
        +"/Users/szc/Desktop/simple_Tetris2/main"+trans_data(my_data0);
        system(s.c_str());
        
        ifstream temp("temp_judge_out");
        int temp_ans=0;
        temp>>temp_ans;
        sum2+=temp_ans;
        temp.close();
    }
    sum=sum1+sum2-last-last0;
    if(random()/double(2147483647)<=double(1)/(1+exp(-sum))){
        *my_data1=*my_mutedata;
        cout<<*my_data1<<" win_rate "<<sum1<<" ("<<sum2<<")"<<endl;
        --step;
        mutation_range*=mutation_rate;
        last0=sum1;
        last=sum2;
        return 1;
    }
    return 0;
}

int main(int argc,char* argv[]) {
    thread_data dat1,dat2,dat0;
    cin>>dat1>>dat2>>dat0;
    cout<<"player1:"<<endl;
    print(dat1);
    cout<<endl;
    cout<<"player2:"<<endl;
    print(dat2);
    cout<<endl;
    bool tag1=1,tag2=1;
    double mutation_range1=mutation_range,mutation_range2=mutation_range;
    while(1){
        if(tag1){
            cout<<"player1:"<<endl;
            tag1=0;
            last=last1;
            mutation_range=mutation_range1;
            last0=0;
    for(int i=0;i<play_round;++i){
        string s="lua bots_c.lua /Users/szc/Desktop/simple_Tetris2/main"+trans_data(&dat1)
        +"/Users/szc/Desktop/simple_Tetris2/main"+trans_data(&dat2);
        system(s.c_str());
        
        ifstream temp("temp_judge_out");
        int temp_ans=0;
        temp>>temp_ans;
        last0+=temp_ans;
        temp.close();
    }
            for(int i=0;i<step;++i){
                thread_data mutated_dat1(dat1);
                mutated_dat1=mutation(mutated_dat1);
                if(play(&dat1,&mutated_dat1,&dat0,&dat2)){
                    tag1=1;
                }
            }
            if(tag1==0)
                cout<<"player1 beats player 2"<<endl;
            last1=last;
            mutation_range1=mutation_range;
        }
        if(tag1&&tag2==0){
            tag2=1;
        }
        if(tag2){
            cout<<"player2:"<<endl;
            tag2=0;
            last=last2;
            mutation_range=mutation_range2;
            last0=0;
    for(int i=0;i<play_round;++i){
        string s="lua bots_c.lua /Users/szc/Desktop/simple_Tetris2/main"+trans_data(&dat2)
        +"/Users/szc/Desktop/simple_Tetris2/main"+trans_data(&dat1);
        system(s.c_str());
        
        ifstream temp("temp_judge_out");
        int temp_ans=0;
        temp>>temp_ans;
        last0+=temp_ans;
        temp.close();
    }
            for(int i=0;i<step;++i){
                thread_data mutated_dat2(dat2);
                mutated_dat2=mutation(mutated_dat2);
                if(play(&dat2,&mutated_dat2,&dat0,&dat1)){
                    tag2=1;
                }
            }
            if(tag2==0)
                cout<<"player2 beats player 1"<<endl;
            last2=last;
            mutation_range2=mutation_range;
        }
        if(tag2&&tag1==0){
            tag1=1;
        }
        if(tag1==0&&tag2==0)
            break;
    }
    cout<<"player1:"<<endl;
    print(dat1);
    cout<<endl;
    cout<<"player2:"<<endl;
    print(dat2);
    cout<<endl;
    return 0;
}