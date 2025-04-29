#include <random>
#include <string>

std::string introduce_noise(const std::string& message) {
  std::string noised_message = message;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, noised_message.size() - 1);

  if (!noised_message.empty()) {
    int index = dis(gen);
    noised_message[index] = noised_message[index] ^ 0x1;
  }
  return noised_message;
}