/**
 * This file is part of EasyRPG Player.
 *
 * EasyRPG Player is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EasyRPG Player is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EasyRPG Player. If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef SWITCH

// Headers
#include "switch_ui.h"
#include "color.h"
#include "graphics.h"
#include "keys.h"
#include "output.h"
#include "player.h"
#include "bitmap.h"
#include <iostream>
#include <sys/iosupport.h>

#include <switch.h>
#include <cstring>
#include <stdio.h>

#ifdef SUPPORT_AUDIO
#include "audio_switch.h"
AudioInterface& NxUi::GetAudio() {
	return *audio_;
}
#endif

namespace {
	const double ticks_per_msec = 19200.0f;
}

static const devoptab_t dotab_null = {
	"null", 0, NULL, NULL, NULL, NULL, NULL, NULL
};

NxUi::NxUi(int width, int height) :
	BaseUi() {

	fullscreen = false;

	gfxInitDefault();

	current_display_mode.width = width;
	current_display_mode.height = height;
	current_display_mode.bpp = 32;
	const DynamicFormat format(
		32,
		0x000000FF,
		0x0000FF00,
		0x00FF0000,
		0xFF000000,
		PF::NoAlpha);
	Bitmap::SetFormat(Bitmap::ChooseFormat(format));
	main_surface = Bitmap::Create(width, height, true, 32);

#ifdef SUPPORT_AUDIO
	audio_.reset(new NxAudio());
#endif

}

NxUi::~NxUi() {
	gfxExit();
}

void NxUi::Sleep(uint32_t time) {
	u64 nsecs = time * 1000000;
	svcSleepThread(nsecs);
}

static inline double u64_to_double(u64 value) {
	return (((double)(u32)(value >> 32)) * 0x100000000ULL + (u32)value);
}

uint32_t NxUi::GetTicks() const {
	double ticks = u64_to_double(svcGetSystemTick());
	u64 msecs = (u64)(ticks / ticks_per_msec);
	return msecs;
}
void NxUi::BeginDisplayModeChange() {
	// no-op
}

void NxUi::EndDisplayModeChange() {
	// no-op
}

void NxUi::Resize(long /*width*/, long /*height*/) {
	// no-op
}

void NxUi::ToggleFullscreen() {
	// no-op
}

void NxUi::ToggleZoom() {
	// no-op
}

bool NxUi::IsFullscreen() {
	return true;
}

void NxUi::ProcessEvents() {
	hidScanInput();
	u32 input = hidKeysHeld(CONTROLLER_P1_AUTO);
	keys[Input::Keys::UP] = (input & KEY_DUP);
	keys[Input::Keys::DOWN] = (input & KEY_DDOWN);
	keys[Input::Keys::RIGHT] = (input & KEY_DRIGHT);
	keys[Input::Keys::LEFT] = (input & KEY_DLEFT);
	keys[Input::Keys::Z] = (input & KEY_A);
	keys[Input::Keys::X] = (input & KEY_B);
	keys[Input::Keys::N1] = (input & KEY_X);
	keys[Input::Keys::LSHIFT] = (input & KEY_Y);
	keys[Input::Keys::F2] = (input & KEY_L);
	keys[Input::Keys::F] = (input & KEY_R);
	keys[Input::Keys::N9] = (input & KEY_SL);
	keys[Input::Keys::N5] = (input & KEY_SR);
	keys[Input::Keys::F12] = (input & KEY_MINUS);
	keys[Input::Keys::ESCAPE] = (input & KEY_PLUS);
}

void NxUi::UpdateDisplay() {
	uint32_t w, h;
	uint8_t *fb = gfxGetFramebuffer(&w, &h);
	
	const DynamicFormat format(
		32,
		0x000000FF,
		0x0000FF00,
		0x00FF0000,
		0xFF000000,
		PF::NoAlpha);
	BitmapRef framebuffer = Bitmap::Create(fb, w, h, w * 4, format);
	framebuffer->StretchBlit(Rect(160, 0, 960, 720), *main_surface, main_surface->GetRect(), Opacity::opaque);
	
	gfxFlushBuffers();
	gfxSwapBuffers();
	gfxWaitForVsync();
}

void NxUi::BeginScreenCapture() {
	CleanDisplay();
}

BitmapRef NxUi::EndScreenCapture() {
	return Bitmap::Create(*main_surface, main_surface->GetRect());
}

void NxUi::SetTitle(const std::string& /* title */) {
	// no-op
}

bool NxUi::ShowCursor(bool /* flag */) {
	return true;
}

#endif
