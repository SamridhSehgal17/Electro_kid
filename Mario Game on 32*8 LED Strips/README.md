# 🎮 Mario-style Dino Jump Game
A fun DIY Mario-style Dino Jump Game built on a 32×8 LED matrix using an Arduino Uno.
This project creates a retro pixel-style obstacle game, where the character jumps over incoming obstacles — perfect for learning basic game logic, LED animations, and timing control.

# 📷 Project Overview
The 32×8 LED matrix is arranged like a horizontal display.
Mario stays on the left side, while obstacles continuously move from the right.

# Library used
Adafruit NeoPixel

## How the game works:

1.Arduino Uno controls all 256 LEDs of the 32×8 matrix.
2.A small Mario sprite is shown near the left side.
3.Obstacles (vertical blocks) spawn on the right side.
4.The screen scrolls left, giving the illusion of running.
5.When you press the button → Mario jumps.
6.If Mario touches an obstacle → Game Over.
A simple and fun LED-matrix arcade game!

## 🛠 Components Used

Arduino Uno
32×8 LED Matrix
1 Push Button (jump control)
5V Power Supply for LEDs
Jumper wires
That’s it! No additional sensors or modules needed

## 🚀 How the Game Works
🎨 LED Control (using Adafruit NeoPixel)
Each frame, the code updates:
Mario position
Obstacle position
LED colors according to matrix mapping
👦 Mario Standing Sprite
Mario stays fixed at the left .
He has two positions:
Standing (normal) → bottom row
Jump → raised row for a short time

There is no running animation — just standing & jumping.

🧱 Obstacle Movement

A vertical bar (2–5 pixels tall) spawns on the right side.
Moves left one column per frame.
When it reaches the left edge and Mario does not jump → game over.
When it exits the screen → new obstacle spawns.
This gives a basic endless scrolling effect.

⬆️ Jump Logic

Button press triggers a jump:
Mario goes to upper position (1 frame up)
Stays airborne for a short delay
Comes back down

Simple physics, perfect for LED games.

💥 Collision Detection
If obstacle’s x-position equals Mario’s x-position and the obstacle height matches Mario's current row → hit detected → game restarts.
