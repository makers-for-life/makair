🦠 COVID-19 Respirator
=====

**3D printable COVID-19 DIY respirator. Aims at helping hospitals cope with a possible shortage of professional respirators during the outbreak.**

We are a distributed team of 50+ contributors, mostly spread through France. Makers, developers, university teachers, researchers and medical teams collaborated on this project. Our testing assembly operations are located in Nantes, France.

If you're new there, please read the explanations below. _Your contributions are much welcome!_

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
- **Venturi valve** (called "Oxygen Mixer" + "Venturi Valve"; 2 test variants);
- **Air filter casing** (called "Filter Box");
- **Pipe adapters** (called "Adapters");

In order to ensure a proper breathing cycle (inhale + exhale), multiple valves need to be connected together to form a circuit. The motors needs to be controlled in harmony so that the air routing between each valve unit is consistent.

# Components

## Mechanics

| Component | Version | Last Changelog | Ready? | Live CAD Models | Contributors |
| --------- | ------- | -------------- | ------ | --------------- | ------------ |
| [Blower](./src/components/blower) | V13 | Increase air compression rate | ❌ | [link](https://a360.co/2UsIdDU) | Cooprint + [@baptistejamin](https://github.com/baptistejamin)
| [Air Transistor](./src/components/air-transistor) | V4 | Moved away from the guillotine system | ✅ | [link](https://a360.co/2wy4eZX) | [@valeriansaliou](https://github.com/valeriansaliou)
| [Pressure Valve](./src/components/pressure-valve) | V3 | Test the "flap" concept and its accuracy | ✅ | none | Sebastien Le Loch
| Cut-Off Valve | V0 | Not started yet | ❌ | none | N/A
| [Oxygen Mixer](./src/components/oxygen-mixer) | V1 | Modeling experiment | ❌ | [link](https://a360.co/2xfmcR0) | [@valeriansaliou](https://github.com/valeriansaliou)
| [Venturi Valve](./src/components/venturi-valve) | V1 | Modeling experiment | ❌ | none | N/A
| [Filter Box](./src/components/filter-box) | V1 | Hermetic testing, make multiple caps according to needs | ❌ | [link](https://a360.co/33CCX4T) | [@eliottvincent](https://github.com/eliottvincent)
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

#### ↘️ Tuesday 24/03

* **🇬🇧 English Video:**

<a href="https://www.youtube.com/watch?v=TJQ1DIsP8ls" title="Play English Video"><img alt="" src="https://img.youtube.com/vi/TJQ1DIsP8ls/0.jpg" height="240"></a>

* [🇫🇷 French Video](https://www.youtube.com/watch?v=kASINWDjnBE)

#### ↘️ Monday 23/03

* **🇫🇷 French Video:**

<a href="https://www.youtube.com/watch?v=MW8I0W8OaoM" title="Play French Video"><img alt="" src="https://img.youtube.com/vi/MW8I0W8OaoM/0.jpg" height="240"></a>

#### ↘️ Sunday 22/03

* [🇬🇧 English Video](https://www.youtube.com/watch?v=GLX5o_y0Vx0)
* [🇫🇷 French Video (short)](https://www.youtube.com/watch?v=GqGbh__ymw4)
* [🇫🇷 French Video (long)](https://www.youtube.com/watch?v=KqBkgTWbIlg)

# Contact Us

Please [open an issue on GitHub](https://github.com/covid-response-projects/covid-respirator/issues/new) to contact us.

# Renders

## Part 1: The "Air Transistor"

🎦 View: [Animation of the "Air Transistor"](./src/components/air-transistor/schemes/Air%20Transistor%20(Animation%201).mp4)

<img alt="Air Transistor Concept" src="./src/components/air-transistor/schemes/Air%20Transistor%20(Render%201).png" height="240">

## Part 2: The "Blower"

🎦 View: [Animation of the "Blower"](./src/components/blower/schemes/Blower%20(Animation%201).mp4)

<img alt="Blower Concept" src="./src/components/blower/schemes/Blower%20(Render%201).png" height="240">

## Part 3: The "Oxygen Mixer"

<img alt="Oxygen Mixer Concept" src="./src/components/oxygen-mixer/schemes/Oxygen%20Mixer%20(Render%201).png" height="240">
