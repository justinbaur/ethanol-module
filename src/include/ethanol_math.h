#pragma once

// GM-style flex-fuel sensor: 50 Hz = 0% ethanol, 150 Hz = 100% ethanol.
// Clamps below 50 Hz to 0 and above 150 Hz to 100.
inline int hz_to_ethanol(int hz) {
  if (hz <= 50) return 0;
  if (hz >= 150) return 100;
  return hz - 50;
}
