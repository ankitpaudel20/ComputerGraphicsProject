
#include "maths.h"
#include "vector"

std::vector<std::vector<double>> generateMatrixDouble(int row, int col);

vec3 calculateNormal(const vec3 &a, const vec3 &b, const vec3 &c);
bool stringStartsWith(std::string a, std::string b);
void SplitString(std::string s, std::vector<float> &v);
void loadModel(std::string filepath, std::vector<std::vector<float>> &vertices,
               std::vector<std::vector<float>> &indices);
std::vector<double> vec3ToVector(vec3 a);
std::vector<double> vec4ToVector(vec4 a);
void draw(const std::vector<std::vector<double>> &cube,
          const std::vector<std::vector<int>> &edgeMatrix);