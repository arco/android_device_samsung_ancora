/*
 * Copyright (C) 2013 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.cyanogenmod.hardware;

import java.io.File;
import org.cyanogenmod.hardware.util.FileUtils;

/*
 * Display RGB intensity calibration (kcal)
 *
 * Exports methods to get the valid value boundaries, the
 * current color values, and a method to set new ones.
 *
 * Values exported by min/max can be the direct values required
 * by the hardware, or a local (to DisplayColorCalibration) abstraction
 * that's internally converted to something else prior to actual use. The
 * Settings user interface will normalize these into a 0-100 (percentage)
 * scale before showing them to the user, but all values passed to/from
 * the client (Settings) are in this class' scale.
 */

public class DisplayColorCalibration {
    private static final String KCAL_PATH = "/sys/class/graphics/fb0/kcal";

    /*
     * All HAF classes should export this boolean.
     * Real implementations must, of course, return true
     */

    public static boolean isSupported() {
        File f = new File(KCAL_PATH);
        return f.exists();
    }

    /*
     * Set the RGB values to the given input triplet. Input is
     * expected to consist of 3 values, space-separated, each to
     * be a value between the boundaries set by get(Max|Min)Value
     * (see below), and it's meant to be locally interpreted/used.
     */

    public static boolean setColors(String colors) {
        return FileUtils.writeLine(KCAL_PATH, colors);
    }

    /*
     * What's the maximum integer value we take for a color
     */

    public static int getMaxValue() {
        return 255;
    }

    /*
     * What's the minimum integer value we take for a color
     */

    public static int getMinValue() {
        return 0;
    }

    /*
     * What's the default integer value we take for a color
     */

    public static int getDefValue() {
        return getMaxValue();
    }

    /*
     * What's the current RGB triplet?
     * This should return a space-separated set of integers in
     * a string, same format as the input to setColors()
     */

    public static String getCurColors() {
        return FileUtils.readOneLine(KCAL_PATH);
    }
}
