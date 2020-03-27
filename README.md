🦠 COVID-19 Respirator (MakAir)
=====

**3D printable COVID-19 DIY respirator. Aims at helping hospitals cope with a possible shortage of professional respirators during the outbreak.**

We are a distributed team of 50+ contributors, mostly spread through France. Makers, developers, university teachers, researchers and medical teams collaborated on this project. Our testing assembly operations are located in Nantes, France.

If you're new there, please read the explanations below. _Your contributions are much welcome!_

![MakAir Logo](./res/brand/logo-readme.png)

---

**⚠️ NOT READY YET, FURTHER INFORMATION WILL COME SHORTLY!**

---

**Shortcut links:**

* [General EN/FR docs on Google Docs](https://docs.google.com/document/d/1l1nt0o93kdfcmPygp1k6NYKrwIb_uN01kcBAfovy2dE/edit)
* [Regular video updates on YouTube](https://www.youtube.com/channel/UCcf_3KXjeJSMz39J6gsyTSg)

---

# Concept

Roughly, the idea is as follow: hospitals will soon lack mechanical artificial respirators. We aim at building a pump, and a valve system (controlled by electronics). This way, the breathing cycle can be enforced by proper air routing through the valve system.

The overall system is composed of sub-components that can be plugged together and wired to form an air circuit, namely:

- **Air pump** (called "Blower");
- **Valve system (routing variant)** (called "Air Transistor");
- **Valve system (depressuring variant)** (called "Pressure Valve");
- **Valve system (two-way variant)** (PEEP valve, called "Two-Way Valve");
- **Venturi valve** (called "Oxygen Mixer");
- **Air filter casing** (called "Filter Box");
- **Pipe adapters** (called "Adapters");

In order to ensure a proper breathing cycle (inhale + exhale), multiple valves need to be connected together to form a circuit. The motors needs to be controlled in harmony so that the air routing between each valve unit is consistent.

# Components

## Mechanics

| Component | Version | Last Changelog | Ready? | Live CAD Models | Contributors |
| --------- | ------- | -------------- | ------ | --------------- | ------------ |
| [Blower](./src/components/blower) | V13 | Increased air compression rate | ❌ | [view model](https://a360.co/2UsIdDU) | Cooprint + [@baptistejamin](https://github.com/baptistejamin)
| [Air Transistor](./src/components/air-transistor) | V4 | Moved away from the guillotine system | ✅ | [view model](https://a360.co/2wy4eZX) | [@valeriansaliou](https://github.com/valeriansaliou)
| [Pressure Valve](./src/components/pressure-valve) | V4 | Smaller size, FDM printing optimizations | ✅ | [view model](https://a360.co/2WIQJRR) | Sebastien Le Loch + [@valeriansaliou](https://github.com/valeriansaliou)
| [Two-Way Valve](./src/components/two-way-valve) | V1 | Initial design | ❌ | [view model](https://a360.co/2QNO9q8) | [@valeriansaliou](https://github.com/valeriansaliou)
| [Oxygen Mixer](./src/components/oxygen-mixer) | V2 | Re-modeled properly | ❌ | [view model](https://a360.co/2UmAecQ) | [@Melkaz](https://github.com/Melkaz)
| [Filter Box](./src/components/filter-box) | V2 | Hermetic testing, make multiple caps according to needs | ❌ | none | Aptatio + [@eliottvincent](https://github.com/eliottvincent)
| [Adapters](./src/components/adapters) | N/A | Add a test lung adapter | ✅ | none | [@valeriansaliou](https://github.com/valeriansaliou)

## Electronics

🚧 _(work in progress)_

## Software

🚧 _(work in progress)_

# Schemes

## General Concept Scheme

![Overview Scheme](./docs/Overview/Overview.png)

## Pneumatic Circuit Scheme

[![Pneumatic Circuit Scheme](./docs/Pneumatics/Pneumatic%20Circuit/Pneumatic%20Circuit.png)](./docs/Pneumatics/Pneumatic%20Circuit/Pneumatic%20Circuit.png)

_You may click on the scheme to enlarge._

# News & Videos

## Live Updates

* Live updates on Telegram: [join channel](https://t.me/joinchat/AAAAAE_99-k7pKZur-GXCQ).

## Checkpoint Videos

You may subscribe to our [YouTube channel](https://www.youtube.com/channel/UCcf_3KXjeJSMz39J6gsyTSg).

#### ↘️ Thursday 26/03

* **🇬🇧 French Video:**

<a href="https://www.youtube.com/watch?v=olhgf6QtDVI" title="Play English Video"><img alt="" src="https://img.youtube.com/vi/olhgf6QtDVI/0.jpg" height="240"></a>

* [🇫🇷 French Video (in-depth)](https://www.youtube.com/watch?v=yDUS2cBBrT4)

#### ↘️ Tuesday 24/03

* [🇬🇧 English Video](https://www.youtube.com/watch?v=TJQ1DIsP8ls)
* [🇫🇷 French Video](https://www.youtube.com/watch?v=kASINWDjnBE)

#### ↘️ Monday 23/03

* [🇫🇷 French Video](https://www.youtube.com/watch?v=MW8I0W8OaoM)

#### ↘️ Sunday 22/03

* [🇬🇧 English Video](https://www.youtube.com/watch?v=GLX5o_y0Vx0)
* [🇫🇷 French Video](https://www.youtube.com/watch?v=GqGbh__ymw4)
* [🇫🇷 French Video (in-depth)](https://www.youtube.com/watch?v=KqBkgTWbIlg)

# Contact Us

Please [open an issue on GitHub](https://github.com/covid-response-projects/covid-respirator/issues/new) to contact us.

# Renders

## The "Air Transistor"

🎦 View: [Animation of the "Air Transistor"](./src/components/air-transistor/schemes/Air%20Transistor%20(Animation%201).mp4)

<img alt="Air Transistor Concept" src="./src/components/air-transistor/schemes/Air%20Transistor%20(Render%201).png" height="240">

## The "Pressure Valve"

<img alt="Pressure Valve Concept" src="./src/components/pressure-valve/schemes/Pressure%20Valve%20(Render%201).png" height="240">

## The "Two-Way Valve"

<img alt="Two-Way Valve Concept" src="./src/components/two-way-valve/schemes/Two-Way%20Valve%20(Render%201).png" height="240">

## The "Blower"

🎦 View: [Animation of the "Blower"](./src/components/blower/schemes/Blower%20(Animation%201).mp4)

<img alt="Blower Concept" src="./src/components/blower/schemes/Blower%20(Render%201).png" height="240">

## The "Oxygen Mixer"

<img alt="Oxygen Mixer Concept" src="./src/components/oxygen-mixer/schemes/Oxygen%20Mixer%20(Render%201).jpg" height="240">

## The "Filter Box"

<img alt="Filter Box Concept" src="./src/components/filter-box/schemes/Filter%20Box%20(Render%201).png" height="320">
