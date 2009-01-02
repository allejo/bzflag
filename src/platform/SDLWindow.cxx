/* bzflag
 * Copyright (c) 1993 - 2008 Tim Riker
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "common.h"

/* interface header */
#include "SDLWindow.h"
#include <iostream>


int SDLWindow::mx = 0;
int SDLWindow::my = 0;


SDLWindow::SDLWindow(const SDLDisplay* _display, SDLVisual*)
  : BzfWindow(_display), width(-1), height(-1), hasGamma(true), iconified(false)
{
}


void SDLWindow::setTitle(const char * title) {
  SDL_WM_SetCaption(title, title);
}


void SDLWindow::setFullscreen(bool on) {
  ((SDLDisplay *)getDisplay())->setFullscreen(on);
}


bool SDLWindow::getFullscreen() const {
  return ((SDLDisplay *)getDisplay())->getFullscreen();
}


void SDLWindow::deiconify() {
  std::cout << "deiconifying" << std::endl;
  iconified = false;

  create();

  if (width != -1 && height != -1) {
    std::cout << "setting the size to " << width << "x" << height << " with fullscreen set to " << getFullscreen() << std::endl;
    setSize(width, height);
    callResizeCallbacks();
  }

  callExposeCallbacks();
}


void SDLWindow::iconify() {
  int x, y;
  std::cout << "iconify toggle" << std::endl;

  if (iconified) {
    deiconify();
    return;
  }

  /* get out of fullscreen so we can unmap */
  if (getFullscreen()) {
    setFullscreen(false);
    callResizeCallbacks();
  }

  getSize(width, height);
  std::cout << "got size " << width << "x" << height << " with fullscreen set to " << getFullscreen() << std::endl;

  setSize(32, 32);
  getSize(x, y);
  std::cout << "after 32 set, got size " << x << "x" << y << " with fullscreen set to " << getFullscreen() << std::endl;

  SDL_WM_IconifyWindow();
  iconified = true;
}


void SDLWindow::warpMouse(int x, int y) {
  SDL_WarpMouse(x, y);
}


void SDLWindow::getMouse(int& x, int& y) const {
  x = mx;
  y = my;
}


void SDLWindow::setSize(int _width, int _height) {
  ((SDLDisplay *)getDisplay())->setWindowSize(_width, _height);
}


void SDLWindow::getSize(int& _width, int& _height) const {
  ((SDLDisplay *)getDisplay())->getWindowSize(_width, _height);
}


void SDLWindow::setGamma(float gamma) {
  int result = SDL_SetGamma(gamma, gamma, gamma);
  if (result == -1) {
    printf("Could not set Gamma: %s.\n", SDL_GetError());
    hasGamma = false;
  }
}


// Code taken from SDL (not available through the headers)
static float CalculateGammaFromRamp(Uint16 ramp[256]) {
  /* The following is adapted from a post by Garrett Bass on OpenGL
     Gamedev list, March 4, 2000.
  */
  float sum = 0.0;
  int count = 0;

  float gamma = 1.0;
  for (int i = 1; i < 256; ++i) {
    if ((ramp[i] != 0) && (ramp[i] != 65535)) {
      double B = (double)i / 256.0;
      double A = ramp[i] / 65535.0;
      sum += (float) (log(A) / log(B));
      count++;
    }
  }
  if ( count && sum ) {
    gamma = 1.0f / (sum / count);
  }
  return gamma;
}


float SDLWindow::getGamma() const {
  Uint16 redRamp[256];
  Uint16 greenRamp[256];
  Uint16 blueRamp[256];
  float gamma = 1.0;
  int result = SDL_GetGammaRamp(redRamp, greenRamp, blueRamp);
  if (result == -1) {
    printf("Could not get Gamma: %s.\n", SDL_GetError());
  } else {
    float red   = CalculateGammaFromRamp(redRamp);
    float green = CalculateGammaFromRamp(greenRamp);
    float blue  = CalculateGammaFromRamp(blueRamp);
    gamma = (red + green + blue) / 3.0;
  }
  return gamma;
}


bool SDLWindow::hasGammaControl() const {
  return hasGamma;
}


void SDLWindow::swapBuffers() {
  SDL_GL_SwapBuffers();
}


bool SDLWindow::create(void) {
  if (!((SDLDisplay *)getDisplay())->createWindow()) {
    return false;
  }
  return true;
}


void SDLWindow::enableGrabMouse(bool on) {
  ((SDLDisplay *)getDisplay())->enableGrabMouse(on);
}


// Local Variables: ***
// mode: C++ ***
// tab-width: 8 ***
// c-basic-offset: 2 ***
// indent-tabs-mode: t ***
// End: ***
// ex: shiftwidth=2 tabstop=8