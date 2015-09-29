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
package xdevs.lib.slide.main;

import xdevs.core.modeling.Atomic;

/**
 *
 * @author José Luis Risco Martín <jlrisco at ucm.es>
 */
public class Host extends Atomic {

    public enum HostPhase {ON, SUSPENDED, OFF, POWERING_ON, SUSPENDING, POWERING_OFF, FAILED}
    
    protected int nCpu = 0;
    protected int nCores = 0;
    protected int coreCapacity = 0;
    protected int memory = 0;
    protected int bandwidth = 0;
    protected int storage = 0;
    protected int privCpu = 0;
    protected int privMemory = 0;
    protected int privBandwidth = 0;
    protected int privStorage = 0;

    public Host(String name, double processingTime) {
        super(name);
        /*super.addInPort(iIn);
         super.addOutPort(oOut);
         this.processingTime = processingTime;*/
    }

    @Override
    public void initialize() {
        super.passivateIn(HostPhase.OFF.toString());
    }

    @Override
    public void deltint() {
        super.passivate();
    }

    @Override
    public void deltext(double d) {
        super.passivate();
    }

    @Override
    public void lambda() {
    }

}
