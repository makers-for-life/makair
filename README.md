MakAir — Covid-19 Respirator
=====

**Mass-producible Covid-19 ARDS respirator. Aims at helping hospitals cope with a possible shortage of professional respirators during the outbreak. Worldwide.**

We are a distributed team of 200+ contributors, mostly spread through France. Makers, developers, university teachers, researchers and medical teams collaborated on this project. Our testing & assembly operations are located in France.

If you're new there, please read the explanations below. _Your contributions are much welcome!_

![MakAir Logo](./res/assets/logo-readme.png)

---

**⚠️ WORK IN PROGRESS — PLEASE DO NOT CONSIDER ALL PROJECT COMPONENTS READY FOR LIVE CLINICAL USE!**

➡️ **Updates:** We are ongoing finalization, endurance testing and final industrialization of our V1 MakAir respirator design. In the meantime, clinical trials have been started. We will provide more information gradually as we know more.

---

**Shortcut links:**

* [General EN/FR docs on Google Docs](https://docs.google.com/document/d/1l1nt0o93kdfcmPygp1k6NYKrwIb_uN01kcBAfovy2dE/edit)
* [Regular video updates on YouTube](https://www.youtube.com/channel/UCcf_3KXjeJSMz39J6gsyTSg)

---

# Concept

Roughly, the idea is as follows: hospitals will soon lack mechanical artificial respirators. We aim at building a pump, and a valve system (controlled by electronics). This way, the breathing cycle can be enforced by proper air routing through the valve system.

The overall system is composed of sub-components that can be plugged together and wired to form an air circuit, namely:

- **Air pump** (called "Blower");
- **Air pump casing fit** (called "Blower Holder");
- **Valve system** (called "Pressure Valve");
- **Oxygen Mixer valve** (called "Oxygen Mixer");
- **Air filter casing (patient variant)** (called "Patient Filter Box");
- **Air filter casing (machine variant)** (called "Machine Filter Box"; 2 directions);
- **Connectors** (called "Pneumatic Connectors");
- **Housing container** (called "Container");

In order to ensure a proper breathing cycle (inhale + exhale), multiple valves need to be connected together to form a circuit. The motors needs to be controlled in harmony so that the air routing between each valve unit is consistent.

This project provides all the parts required to build a good-enough [ARDS](https://en.wikipedia.org/wiki/Acute_respiratory_distress_syndrome) respirator from mass-produced components. We provide all the required mechanical parts, electronics designs & boards, and firmwares. This respirator can be 3D-printed and ran on an Arduino board (the maker way), though we **highly advise** that you work with industrial processes as to mold medical-grade plastic parts and assemble the whole respirator (this would be required for the built respirator to pass all medical certifications).

We target a per-unit cost well under 500 EUR, which could easily be shrunk down to 200 EUR or even 100 EUR per respirator given proper economies of scale, as well as choices of cheaper on-the-shelf components (eg. servomotors).

_We re-iterate: though 3D-printing (FDM and SLA) can be used to build your own respirator — this will definitely not scale well to mass-produce MakAir respirators, and parts might be brittle or leak air. Please work with proper industrial methods and processes if you want to build your own MakAir respirators._

# Components

## Mechanics

| Part | Version | Last Changelog | Tested? | Live CAD Models | Contributors |
| ---- | ------- | -------------- | ------ | --------------- | ------------ |
| [Blower](./src/mechanics/parts/blower) | V14 | Smaller form factor & more powerful | ❌ | [view model](https://a360.co/39JpHg9) | Gabriel Moneyron + [@baptistejamin](https://github.com/baptistejamin) + [@valeriansaliou](https://github.com/valeriansaliou)
| [Blower Holder](./src/mechanics/parts/blower-holder) | V1 | Initial version | ✅ | [view core model](https://a360.co/2X7eoLW) + [view casing model](https://a360.co/2X5DwCR) | Gabriel Moneyron + [@valeriansaliou](https://github.com/valeriansaliou)
| [Pressure Valve](./src/mechanics/parts/pressure-valve) | V5 | Build fixes | ✅ | [view model](https://a360.co/2xOa2ic) | [@clementniclot](https://github.com/clementniclot)
| [Oxygen Mixer](./src/mechanics/parts/oxygen-mixer) | V4 | Updated with correct O2 ID diameter, reinforced O2 nipple | ❌ | [view model](https://a360.co/345fsS2) | [@Melkaz](https://github.com/Melkaz)
| [Patient Filter Box](./src/mechanics/parts/patient-filter-box) | V6 | Update input/output mensurations | ❌ | [view model](https://a360.co/2X5VvJq) | Martial Medjber + [@eliottvincent](https://github.com/eliottvincent)
| [Machine Filter Box (Intake)](./src/mechanics/parts/machine-filter-box) | V1 | Wall-mountable, 3D-printability optimizations | ✅ | [view model](https://a360.co/2wZids4) | [@valeriansaliou](https://github.com/valeriansaliou)
| [Machine Filter Box (Outtake)](./src/mechanics/parts/machine-filter-box) | V1 | Wall-mountable, 3D-printability optimizations | ✅ | [view model](https://a360.co/2JyLaxG) | [@valeriansaliou](https://github.com/valeriansaliou)
| [Pneumatic Connectors](./src/mechanics/parts/pneumatic-connectors) | V2 | Initial version | ✅ | none | Gabriel Moneyron
| [Container](./src/mechanics/container) | V2 | Update mensurations | ❌ | [view model](https://a360.co/3dVmJbR) | Arthur Dagard

## Electronics

| Board | Version | Major Changes | Tested? | Contributors |
| ----- | ------- | ------------- | ------ | ------------ |
| [Motherboard](./src/electronics/motherboard) | V1.1 | Working PCB w/ software | ✅ | Tronico + [@pi-r-p](https://github.com/pi-r-p)

## Software

| Runtime | Version | Major Changes | Tested? | Contributors |
| ------- | ------- | ------------- | ------ | ------------ |
| [Respiratory Firmware](./src/arduino) | V1.1.x | Initial test working | ✅ | [@Mefl](https://github.com/Mefl) + [@jabby](https://github.com/jabby) + [@dsferruzza](https://github.com/dsferruzza)

# Schemes

## Pneumatic Circuit Scheme

[![Pneumatic Circuit Scheme](./docs/Pneumatics/Pneumatic%20Circuit/Pneumatic%20Circuit.png)](./docs/Pneumatics/Pneumatic%20Circuit/Pneumatic%20Circuit.png)

_(design by [@valeriansaliou](https://github.com/valeriansaliou))_

## Container Layout

### Top Part: Electronics (Power & Controllers)

[![Container Layout Electronics Drawing](./docs/Container/Container%20Layout/Container%20Layout%20Electronics.jpg)](./docs/Container/Container%20Layout/Container%20Layout%20Electronics.jpg)

_(design by Arthur Dagard; drawing by [@valeriansaliou](https://github.com/valeriansaliou))_

### Bottom Part: Pneumatics

[![Container Layout Pneumatics Drawing](./docs/Container/Container%20Layout/Container%20Layout%20Pneumatics.jpg)](./docs/Container/Container%20Layout/Container%20Layout%20Pneumatics.jpg)

_(design by Arthur Dagard; drawing by [@valeriansaliou](https://github.com/valeriansaliou))_

# News & Contact

* Live updates on Telegram: [join channel](https://t.me/joinchat/AAAAAE_99-k7pKZur-GXCQ).
* Contact us: [open an issue on GitHub](https://github.com/covid-response-projects/covid-respirator/issues/new).

# Renders

## Mechanics

### The "Pressure Valve"

<p>
  <img alt="Pressure Valve Render" src="./src/mechanics/parts/pressure-valve/fdm/schemes/Pressure%20Valve%20(Render%201).png" height="240">
  <img alt="Pressure Valve Print" src="./src/mechanics/parts/pressure-valve/fdm/schemes/Pressure%20Valve%20(Print%201).jpg" height="240">
</p>

### The "Blower"

🎦 View: [Animation of the "Blower"](./src/mechanics/parts/blower/fdm/schemes/V14/Blower%20(Animation%201).mp4)

<p>
  <img alt="Blower Render" src="./src/mechanics/parts/blower/fdm/schemes/V14/Blower%20(Render%201).png" height="240">
  <img alt="Blower Render" src="./src/mechanics/parts/blower/fdm/schemes/V14/Blower%20(Render%202).png" height="240">
  <img alt="Blower Render" src="./src/mechanics/parts/blower/fdm/schemes/V14/Blower%20(Print%201).jpg" height="240">
</p>

### The "Blower Holder"

<p>
  <img alt="Blower Holder Render" src="./src/mechanics/parts/blower-holder/fdm/schemes/V1/Blower%20Holder%20(Render%201).png" height="240">
  <img alt="Blower Holder Print" src="./src/mechanics/parts/blower-holder/fdm/schemes/V1/Blower%20Holder%20(Print%201).jpg" height="240">
</p>

### The "Oxygen Mixer"

<p>
  <img alt="Oxygen Mixer Render" src="./src/mechanics/parts/oxygen-mixer/fdm/schemes/V4/Oxygen%20Mixer%20(Render%201).png" height="240">
  <img alt="Oxygen Mixer Print" src="./src/mechanics/parts/oxygen-mixer/fdm/schemes/V4/Oxygen%20Mixer%20(Print%201).jpg" height="240">
</p>

### The "Machine Filter Box" (Both Directions)

<p>
  <img alt="Machine Filter Box Render" src="./src/mechanics/parts/machine-filter-box/fdm/schemes/V1/Filter%20Box%20(Render%201).png" height="320">
  <img alt="Machine Filter Box Print" src="./src/mechanics/parts/machine-filter-box/fdm/schemes/V1/Filter%20Box%20(Print%201).jpg" height="320">
</p>

### The "Patient Filter Box"

<p>
  <img alt="Patient Filter Box Render" src="./src/mechanics/parts/patient-filter-box/fdm/schemes/V6/Filter%20Box%20(Mensurations).JPG" height="320">
  <img alt="Machine Filter Box Print" src="./src/mechanics/parts/patient-filter-box/fdm/schemes/Archives/V5/Filter%20Box%20(Print%201).jpg" height="320">
</p>

### The "Pneumatic Connectors"

<p>
  <img alt="Pneumatic Connector Blower Render" src="./src/mechanics/parts/pneumatic-connectors/fdm/schemes/V2/Blower%20V5/Pneumatic%20Connector%20Blower%20V5.png" height="240">
  <img alt="Pneumatic Connector O2 Render" src="./src/mechanics/parts/pneumatic-connectors/fdm/schemes/V2/O2%20V3/Pneumatic%20Connector%20O2%20V3.png" height="240">
  <img alt="Pneumatic Connector No pressure Render" src="./src/mechanics/parts/pneumatic-connectors/fdm/schemes/V2/Respiratory%20-%20No%20pressure/Pneumatic%20Connector%20Respiratory%20No%20pressure.png" height="240">
  <img alt="Pneumatic Connector pressure Render" src="./src/mechanics/parts/pneumatic-connectors/fdm/schemes/V2/Respiratory%20-%20Pressure%20V2/Pneumatic%20Connector%20Respiratory%20Pressure%20V2.png" height="240">
  <img alt="Pneumatic Connector O2 Print" src="./src/mechanics/parts/pneumatic-connectors/fdm/schemes/V2/O2%20V3/Pneumatic%20Connector%20O2%20V3%20Print.jpg" height="240">
  <img alt="Pneumatic Connector Blower Print" src="./src/mechanics/parts/pneumatic-connectors/fdm/schemes/V2/Blower%20V5/Pneumatic%20Connector%20Blower%20V4%20Print.jpg" height="240">
  <img alt="Pneumatic Connector No pressure Print" src="./src/mechanics/parts/pneumatic-connectors/fdm/schemes/V2/Respiratory%20-%20No%20pressure/Pneumatic%20Connector%20Respiratory%20No%20pressure%20Print.jpg" height="240">
</p>

### The "Container"

<p>
  <img alt="Container Render" src="./src/mechanics/container/acrylic/schemes/V2/Container%20(Render%201).png" height="240">
  <img alt="Container Render" src="./src/mechanics/container/acrylic/schemes/V2/Container%20(Render%202).png" height="240">
  <img alt="Container Render" src="./src/mechanics/container/acrylic/schemes/V2/Container%20(Render%204).png" height="240">
  <img alt="Container Render" src="./src/mechanics/container/acrylic/schemes/V2/Container%20(Render%206).png" height="240">
  <img alt="Container Render" src="./src/mechanics/container/acrylic/schemes/V2/Container%20(Render%207).png" height="240">
  <img alt="Container Render" src="./src/mechanics/container/acrylic/schemes/V2/Container%20(Render%208).png" height="240">
</p>

## Electronics

### The "Motherboard"

<p>
  <img alt="Motherboard Picture" src="./src/electronics/motherboard/schemes/V1/Motherboard%20(Picture%201).jpg" height="240">
</p>
