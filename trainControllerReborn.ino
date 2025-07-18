// 引脚定义
const int handlePin = A0;// 模拟输入引脚，连接电位器（调速旋钮）
const int PWM  = 3;      // PWM 引脚，连接至 L298N 模块用于速度控制
const int PWMD = 5;      // 方向控制引脚 D，连接至 L298N
const int PWMR = 6;      // 方向控制引脚 R，连接至 L298N
const int dirD = 7;      // 方向开关的输入引脚 D
const int dirR = 8;      // 方向开关的输入引脚 R

// 控制变量
int sensorValue = 0;     // 电位器读取值（用于计算目标速度）
int currentSpeed = 0;    // 当前速度（带正负，表示方向）
int targetSpeed = 0;     // 目标速度（带正负，表示方向）
int direction = 0;       // 当前方向：1（正向），-1（反向），0（停止）

unsigned long previousSpeedUpdate = 0;  // 上一次速度更新时间
const unsigned long speedInterval = 15; // 每隔15ms更新一次速度

// 设置引脚模式
void setup() {
  pinMode(PWMD , OUTPUT);
  pinMode(PWMR , OUTPUT);
  pinMode(dirD , INPUT_PULLUP);
  pinMode(dirR , INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();
  sensorValue = analogRead(handlePin) / 4;
  direction = readDirection();
  targetSpeed = sensorValue * direction;

  // 非阻塞式速度控制
  if (currentMillis - previousSpeedUpdate >= speedInterval) {
    previousSpeedUpdate = currentMillis;
    updateSpeed();
  }
  setMotorDirection();
  applyPWM();
}

// 读取方向
int readDirection() {
  if (digitalRead(dirD) == HIGH && digitalRead(dirR) == LOW) {
    return 1;
  } else if (digitalRead(dirD) == LOW && digitalRead(dirR) == HIGH) {
    return -1;
  } else {
    return 0;
  }
}

// 更新当前速度
void updateSpeed() {
  if (targetSpeed < currentSpeed) {
    currentSpeed--;
  } else if (targetSpeed > currentSpeed) {
    currentSpeed++;
  }
}

// 设置方向引脚
void setMotorDirection() {
  if (currentSpeed > 0) {
    digitalWrite(PWMD , HIGH);
    digitalWrite(PWMR , LOW);
  } else if (currentSpeed < 0) {
    digitalWrite(PWMR , HIGH);
    digitalWrite(PWMD , LOW);
  } else {
    digitalWrite(PWMD , LOW);
    digitalWrite(PWMR , LOW);
  }
}

// 设置 PWM
void applyPWM() {
  analogWrite(PWM, constrain(abs(currentSpeed), 0, 255));
}
