/*
 * Copyright (C) 2015-2015 GreenDisc research group <http://greendisc.dacya.ucm.es/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contributors:
 *  - José Luis Risco Martín
 *  - Marina Zapater Sancho
 */
package xdevs.lib.slide.coupled.examples;

import java.util.LinkedList;
import xdevs.core.modeling.Coupled;
import xdevs.core.simulation.Coordinator;

/**
 *
 * @author José Luis Risco Martín <jlrisco at ucm.es>
 */
public class SimpleDataCenter extends Coupled {
    // I have clusters
    public static void main(String args[]) {
        SimpleDataCenter dataCenter = new SimpleDataCenter();
        Coordinator devsSimulator = new Coordinator(dataCenter);
        devsSimulator.initialize();
        devsSimulator.simulate(600.0);
    }
}
