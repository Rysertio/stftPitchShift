#pragma once

#include <vector>

class StftPitchShift
{

public:

  StftPitchShift(
    const size_t framesize,
    const size_t hopsize,
    const float samplerate);

  void shiftpitch(
    const std::vector<float>& input,
    std::vector<float>& output,
    const std::vector<float>& factors = {1},
    const float quefrency = 0);

private:

  const size_t framesize;
  const size_t hopsize;
  const float samplerate;

};