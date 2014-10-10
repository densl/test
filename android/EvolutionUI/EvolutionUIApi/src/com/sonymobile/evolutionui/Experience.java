/*
 * Copyright (C) 2014 Sony Mobile Communications AB
 *
 * This file is part of EvolutionUI.
 *
 * EvolutionUI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * EvolutionUI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EvolutionUI. If not, see <http://www.gnu.org/licenses/>.
 */
package com.sonymobile.evolutionui;

/**
 * The experience represents an action which the user can execute.
 * <p>
 * By executing that action, he gains experience.
 * For example making a phone call can be an experience.
 */
public class Experience extends BasicConcept {

    public Experience(String name) {
        setName(name);
    }

    @Override
    public String getIconRes() {
        // Experiences are not shown in the UI, so they don't need icons.
        return null;
    }

    @Override
    public String getDefaultIconRes() {
        // Experiences are not shown in the UI, so they don't need icons.
        return null;
    }
}
