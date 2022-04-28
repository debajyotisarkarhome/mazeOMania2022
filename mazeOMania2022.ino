#include <HCSR04.h>

#include <StackArray.h>


//pin definations
//Motor1 goes to left

StackArray <int> nodeStack;

#define M11 2
#define M12 3
#define M1S A0


#define M21 4
#define M22 5
#define M2S A1

#define S11 6
#define S12 7

HCSR04 leftSensor(S11, S12);

#define S21 8
#define S22 9

HCSR04 frontSensor(S21, S22);

#define S31 10
#define S32 11

HCSR04 rightSensor(S31, S32);

#define IRS1 12
#define IRS2 13

#define unitIRhit=4

#define MAX_DRIVE 20
#define MAX_TURN 15
#define SAFE_DIST 10 //cm
#define INF 999


int counter=0;
int IRset=0;
int IRsignal;
int visitedArray[48];
int hitArray[3];
int currentPOS=0;

char orientationArray[4]={'^','>','v','<'};
int orientation=0;

//----------------------------------------------------------------------
int coun(int arr[]){
  int counter=0;
  for(int i=0;i<3;i++){
    if(arr[i]==1){
      counter++;
    }
  }
  return counter;
}

void stopp(){
  digitalWrite(M11,LOW);
  digitalWrite(M12,LOW);

  digitalWrite(M21,LOW);
  digitalWrite(M22,LOW);
}

int forwardCalc(int currentPOS[],int orientation){
  int x=currentPOS[0];
  int y=currentPOS[1];
  if(orientation=1)
    y=y+1;
  else if(orientation=2)
    x=x+1;
  else if(orientation=3)
    y=y-1;
  else if(orientation=4)
    x=x-1;
  return (y*7)+x;
}

void forward(){
  counter=0;
  digitalWrite(M11,HIGH);
  digitalWrite(M12,LOW);
  digitalWrite(M21,HIGH);
  digitalWrite(M22,LOW);
  while(counter<MAX_DRIVE){
    IRsignal=digitalRead(12);
    if(IRset!=IRsignal){
      counter++;
      IRset=IRsignal;
    }
  }
  stopp();
}

int backwardCalc(int currentPOS[],int orientation){
  int x=currentPOS[0];
  int y=currentPOS[1];
  if(orientation=1)
    y=y-1;
  else if(orientation=2)
    x=x-1;
  else if(orientation=3)
    y=y+1;
  else if(orientation=4)
    x=x+1;
  return (y*7)+x;
}

void backward(){
  counter=0;
  digitalWrite(M11,LOW);
  digitalWrite(M12,HIGH);

  digitalWrite(M21,LOW);
  digitalWrite(M22,HIGH);
  while(counter<MAX_DRIVE){
    IRsignal=digitalRead(12);
    if(IRset!=IRsignal){
      counter++;
      IRset=IRsignal;
    }
  }
  stopp();
}

int rightCalc(int currentPOS[],int orientation){
  int x=currentPOS[0];
  int y=currentPOS[1];
  if(orientation==1)
      x=x+1;
  else if(orientation==2)
      y=y-1;
  else if(orientation==3)
      x=x-1;
  else if(orientation==4)
      y=y+1;
  return (y*7)+x;
}

void right(){
  counter=0;
  digitalWrite(M11,HIGH);
  digitalWrite(M12,LOW);

  digitalWrite(M21,LOW);
  digitalWrite(M22,HIGH);
  while(counter<MAX_TURN){
    IRsignal=digitalRead(12);
    if(IRset!=IRsignal){
      counter++;
      IRset=IRsignal;
    }
  }
  stopp();
}

int leftCalc(int currentPOS[],int orientation){
  int x=currentPOS[0];
  int y=currentPOS[1];
  if(orientation==1)
      x=x-1;
  else if(orientation==2)
      y=y+1;
  else if(orientation==3)
      x=x+1;
  else if(orientation==4)
      y=y-1;
  return (y*7)+x;
}

void left(){
  counter=0;
  digitalWrite(M11,LOW);
  digitalWrite(M12,HIGH);

  digitalWrite(M21,HIGH);
  digitalWrite(M22,LOW);
  while(counter<MAX_TURN){
    IRsignal=digitalRead(12);
    if(IRset!=IRsignal){
      counter++;
      IRset=IRsignal;
    }
  }
  stopp();
}



//----------------------------------------------------------------------






void setup(){
  //int counter;
  memset(visitedArray,0,sizeof(visitedArray));
  visitedArray[0]=1;
  Serial.begin(9600);
  for(int i=0;i<=5;i++){
    pinMode(i,OUTPUT);
  }
  analogWrite(A0,1023);
  analogWrite(A1,1023);
}

void loop(){
  if(visitedArray[32]==1){
    stopp();
  }
  analogWrite(A0,1023);
  analogWrite(A1,1023);
  hitArray[0]=(leftSensor.dist()<SAFE_DIST);
  hitArray[1]=(frontSensor.dist()<SAFE_DIST);
  hitArray[2]=(rightSensor.dist()<SAFE_DIST);

  if(coun(hitArray)==3){
    int lastDecisionCell=nodeStack.pop();
    while(currentPOS!=lastDecisionCell){
      hitArray[0]=(leftSensor.dist()<SAFE_DIST);
      hitArray[1]=(frontSensor.dist()<SAFE_DIST);
      hitArray[2]=(rightSensor.dist()<SAFE_DIST);
      if(hitArray[0]!=1){
        left();
        forward();
        currentPOS=leftCalc(currentPOS,orientation);
        visitedArray[currentPOS]=1;
        if(orientation==1)
          orientation=4;
        else
          orientation=orientation-1;
      }
      else if(hitArray[1]!=1){
        currentPOS=forwardCalc(currentPOS,orientation);
        visitedArray[currentPOS]=1;
        
      }
      else if(hitArray[2]!=1){
        right();
        forward();
        currentPOS=rightCalc(currentPOS,orientation);
        visitedArray[currentPOS]=1;
        if(orientation==4)
          orientation=1;
        else
          orientation=orientation+1;
      }
    }
  }
  
  if(coun(hitArray)==2){
    nodeStack.push(currentPOS);
    if(hitArray[0]!=1){
      left();
      forward();
      currentPOS=leftCalc(currentPOS,orientation);
      visitedArray[currentPOS]=1;
      if(orientation==1)
        orientation=4;
      else
        orientation=orientation-1;
    }
    else if(hitArray[1]!=1){
      currentPOS=forwardCalc(currentPOS,orientation);
      visitedArray[currentPOS]=1;
    }
    else if(hitArray[2]!=1){
      right();
      forward();
      currentPOS=rightCalc(currentPOS,orientation);
      visitedArray[currentPOS]=1;
      if(orientation==4)
        orientation=1;
      else
        orientation=orientation+1;
    }
  }

    ///Simple traversal End
    else{
      int POS=(currentPOS);
      visitedArray[POS]=INF;
      nodeStack.push(POS);

      if(hitArray[1]!= 1 && visitedArray[forwardCalc(currentPOS,orientation)!=1]){
        forward(); 
        currentPOS=forwardCalc(currentPOS,orientation);
        visitedArray[currentPOS]=1;
      }
      else if(hitArray[2]!=1 && visitedArray[rightCalc(currentPOS,orientation)!=1]){
        right();
        forward();
        currentPOS=rightCalc(currentPOS,orientation);
        visitedArray[currentPOS]=1;
        if(orientation==4)
          orientation=1;
        else
          orientation=orientation+1;
      }
      else if(hitArray[0]!=1 && visitedArray[leftCalc(currentPOS,orientation)!=1]){
        left();
        forward();
        currentPOS=leftCalc(currentPOS,orientation);
        visitedArray[currentPOS]=1;
        if(orientation==1)
          orientation=4;
        else
          orientation=orientation-1;
      }
  }
}.
