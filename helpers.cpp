#include "helpers.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

bool stringStartsWith(std::string a, std::string b) {

  if (a.rfind(b, 0) == 0) {
    return true;
  }
  return false;
}
void SplitString(std::string s, std::vector<float> &v) {

  std::string temp = "";
  for (int i = 0; i < s.length(); ++i) {

    if (s[i] == ' ') {
      if (temp != "v" and temp != "f" and temp != "") {
        v.push_back(std::stof(temp));
      }
      temp = "";
    } else {
      temp.push_back(s[i]);
    }
  }
  v.push_back(std::stof(temp));
}

void loadModel(std::string filepath, std::vector<std::vector<float>> &vertices,
               std::vector<std::vector<float>> &indices) {
  /* read vertex data and index from the file */
  std::ifstream file(filepath);
  std::string str;

  while (std::getline(file, str)) {
    // process string ...
    if (stringStartsWith(str, "v")) {
      std::vector<float> vertex;
      SplitString(str, vertex);
      vertices.push_back(vertex);
      // std::cout << str << std::endl;
    }
    if (stringStartsWith(str, "f")) {
      std::vector<float> index;
      SplitString(str, index);
      indices.push_back(index);
      // std::cout << str << std::endl;
    }
  }
}
std::vector<double> vec3ToVector(vec3 a) {
  std::vector<double> b;
  b.push_back(a.x);
  b.push_back(a.y);
  b.push_back(a.z);
  return b;
}
std::vector<double> vec4ToVector(vec4 a) {
  std::vector<double> b;
  b.push_back(a.x);
  b.push_back(a.y);
  b.push_back(a.z);
  b.push_back(a.w);
  return b;
}

vec3 calculateNormal(const vec3 &a, const vec3 &b, const vec3 &c) {
  vec3 u = b - a;
  vec3 v = c - a;
  return vec3::cross(u, v);
}

std::vector<std::vector<double>> generateMatrixDouble(int row, int col) {

  std::vector<std::vector<double>> result(row);
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {

      int randomNum = rand() % 20 - 10;
      double castedVal = (double)randomNum;
      result[i].push_back(randomNum);
    }
  }
  return result;
}