#include <LiquidCrystal.h>
#include "lcd.h"
#include "config.h"
#include "game.h"
#include "MicroFlow.h"

int topology[] = {1, 16, 16, 1};
double weights[] = {1.8146323, 4.1916428, -1.544234, 1.6016129, -0.9926331, -0.499008, -1.0084211, -1.583112, -1.4041698, -1.9004874, -1.6277786, 3.479049, 3.1480942, -0.4713373, 0.54268444, 1.4436228, 0.2995118, 0.2627033, 0.1893768, -0.39262232, -0.37633345, -0.20294805, 0.26976493, 0.7271523, 0.22095276, 0.20945868, 0.11195129, 0.3102565, 0.045573853, -0.53100145, -0.11398321, 0.44325456, 0.49617094, -0.13911946, 0.89193153, -0.7223627, -0.121638685, -0.006401307, -0.5858811, 0.3646298, -0.48332092, -0.89355654, -0.492423, -0.0071297446, 0.60112524, -1.3437526, -0.82807356, -0.53535104, 0.058349274, -0.5212282, -0.2459296, -0.24881037, 1.0764554, 0.3886957, -0.29132187, -0.754857, -0.56640065, -0.048261683, -0.2200853, -0.11557769, -0.35733467, 0.5267154, 0.28870144, -0.5185784, -0.19033235, 0.5199498, -0.19053394, -0.3244507, -0.26043257, -0.38239312, 0.042729203, 0.8991102, 0.1806766, 0.0028195654, 0.1674506, -0.41516805, 0.41614047, -0.60113364, 0.3274143, -0.124684155, -0.38329583, -0.46212405, 0.22760624, -0.147346, 0.43440992, 0.14778985, -0.3025233, -0.43429607, 0.32889435, 0.4105814, 0.08679163, -0.21720077, 0.24391471, -0.08149339, 0.2133798, -0.123312, -0.40540388, 0.07414649, -0.4228704, 0.35623848, -0.05995661, -0.41341835, -0.18041188, -0.33539766, 0.33801106, -0.10898657, 0.4685575, -0.18488038, -0.36288673, 0.22731797, 0.22698343, -0.14258695, -0.1286931, 0.13444054, -0.089116275, 0.110107616, -0.107536204, 0.31709838, -0.3537884, -0.6263992, -0.28835633, 0.018136492, 0.28509927, -0.44942904, 0.2216462, 0.31597885, 0.38589215, 0.019418625, -0.2773655, -0.26945686, 0.46545961, -0.484632, 0.9332947, 0.22977646, -0.12453228, -1.4787946, -0.3399927, 0.39599088, -0.287267, -0.38730505, -0.45697644, 0.3901685, 0.39287314, -0.014936349, 0.49634135, -0.122734234, -0.08539116, -0.07993663, 0.9841004, 0.38838667, 0.079701625, -1.4036312, -0.39626452, 0.43135118, -0.20828499, 0.06358132, -0.452605, 0.38201076, -0.2773798, -0.37102664, 0.120090395, -0.2843058, 0.19307205, 0.13890058, 1.3630718, 0.497811, 0.17636694, -1.5888776, -0.32145864, -0.27318403, -0.6560665, 0.0059172553, -0.14416918, 0.63844323, -0.30588576, -0.15401231, -0.064376265, -0.43325123, -0.17154671, 0.01649641, 0.24337816, 0.32004738, -0.018320257, -1.1718352, 0.16960526, 0.090278946, -0.45779967, -0.42693678, 0.14088328, 0.1103232, 0.37573618, 0.013675398, 0.783342, -0.22862174, 0.13046512, -0.028544579, -0.24320814, 0.6423921, 0.17587201, -0.084981605, -0.2424032, -0.8871768, -0.3726177, -0.11078385, 0.8284682, -1.0670705, -0.57978076, -0.6733328, 0.6214043, -0.37720492, 0.6603272, -0.22102712, -0.39584646, 0.18717963, 0.15299049, 0.1880163, -0.19488811, -0.6903183, -0.34816006, -0.4158249, 0.33706638, -1.0131887, -0.72506243, 0.14115435, -0.36999714, 0.39335862, 0.017588932, 0.16204546, 0.36300904, -0.46290818, -0.22144388, -0.4000443, 0.49595475, 0.5887392, 0.79325026, 0.18099536, 0.05909953, 0.55795443, 0.44781753, 0.26693934, -0.13921462, 0.033556756, -0.06911224, -0.27000824, 0.1989559, -0.30510622, 0.013521509, 0.44364512, -0.4713988, 0.013111462, -0.36627126, -0.5149059, 0.28342244, -0.043552767, -0.17144926, -0.05098645, -0.10680276, -0.34809715, -0.33043483, 0.17950593, -0.23347719, 0.22093683, -0.26734778, 0.5688992, 0.09719613, -0.109286025, 0.22880675, -0.2707053, 0.2393277, 0.067379385, -0.43376014, 0.044631474, 0.44186214, -0.5032985, 0.61703557, -0.27308142, 0.54936993, 0.5844767, -0.0036831147, -0.8649465, -0.55866915, -0.28584155, -0.44762754, -0.2187668, 0.38921225, -0.75177217, -0.6768076, -0.5306761}; 
double biases[] = {-1.3163362, -1.2545422, 0.8838964, -1.0553644, 0.14953831, 0.03172869, 0.24036531, 1.0583308, 0.7801871, 1.7244657, 1.1930373, -0.6630234, -0.5748427, 0.2706816, -0.20697892, -0.542049, 0.015436362, -0.004821891, -0.04576476, -0.022555908, -0.023150321, 0.0056098294, -0.0578759, -0.0014256665, 0.026579443, 0.08214359, 0.19863725, -0.046731897, 0.009111076, 0.055833265, 0.025399242, 0.04620831, 0.08913628};
double inputs[] = {0};
double outputs[] = {0};
int layers = 4;
MicroMLP mlp(layers, topology, weights, biases, SIGMOID);
void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  initLCD();
  initButtons();

  gameInit();

  #if !defined(AUTO_PLAY)
  startScreen();
  #endif

  //Neural network stuff
  
  
}

void loop() {
  if (gameTick()){
    #if defined(AUTO_PLAY)
    if (distanceToNextCacti() == 1) {
      gameJump();
    }
    delay(gameTickDelay);
    #else
    
    inputs[0] = distanceToNextCacti();
    mlp.feedforward(inputs, outputs);
    Serial.println(outputs[0]);
    
    unsigned long startTickDelay = millis();
    bool jumped = false;
    if (round(outputs[0]) == 1){
          gameJump();
          Serial.println("Jump!");
    }
    while (millis() - startTickDelay < gameTickDelay) {
//      if (readButtons() != NONE) {
//        gameJump();
//        jumped = true;
//      }
    }
    #endif
//    if (jumped){
//      Serial.print(distanceToNextCacti());Serial.print(",");Serial.print("1");
//    }
//    else{
//      Serial.print(distanceToNextCacti());Serial.print(",");Serial.print("0");
//    }
//    Serial.println(" ");
    
  }
  else{
    gameInit();
    startScreen();
  }
}
