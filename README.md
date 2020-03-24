🦠 COVID-19 Respirator
=====

**3D printable COVID-19 DIY respirator. Aims at helping hospitals cope with a possible shortage of professional respirators during the outbreak.**

NOT READY YET, FURTHER INFORMATION WILL COME SHORTLY

Makers, developers, university teachers / researchers and medical teams collaborated on this project. If you're new there, please read the explanations below. _Your contributions are much welcome!_


Global EN/FR doc here https://docs.google.com/document/d/1l1nt0o93kdfcmPygp1k6NYKrwIb_uN01kcBAfovy2dE/edit#

Regular video update here https://www.youtube.com/channel/UCcf_3KXjeJSMz39J6gsyTSg

---

# Concept

Roughly, the idea is as follow: hospitals will soon lack mechanical artificial respirators. We aim at building a pump, and a valve system (controlled by electronics). This way, the breathing cycle can be enforced by proper air routing through the valve system.

The overall system is composed of sub-components that can be plugged together and wired to form an air circuit, namely:

- **The air pump** (called "Blower");
- **The valve system** (called "Air Transistor");

In order to ensure a proper breathing cycle (inhale + exhale), multiple valves need to be connected together to form a circuit. The motors needs to be controlled in harmony so that the air routing between each valve unit is consistent.

# Status

| Component | Version | Team's focus | Ready? | Live CAD Models |
| --------- | ------- | ------------ | ------ | ----------------- |
| [Blower](https://github.com/covid-response-projects/covid-respirator/tree/master/src/components/Blower) | V13 | Increase air compression rate | ❌ | [link](https://a360.co/2UsIdDU)
| [Air Transistor](https://github.com/covid-response-projects/covid-respirator/tree/master/src/components/air-transistor) | V4 (Experimental) | Reduce dead space | ❌ | [link](https://a360.co/2wy4eZX)
| [Air Transistor Valve](https://github.com/covid-response-projects/covid-respirator/tree/master/src/components/air-transistor-valve) | V3 | Test the "flap" concept and its accuracy | ❌ | none
| [Filter box](https://github.com/covid-response-projects/covid-respirator/tree/master/src/components/filter-box) | V1 | Hermetic testing, make multiple caps according to needs | ❌ | [link](https://a360.co/33CCX4T)


# Process

![Overview scheme](./Overview/Overview.png)

# News & Videos

## Updates

* Live updates on Telegram: [join channel](https://t.me/joinchat/AAAAAE_99-k7pKZur-GXCQ).

## Videos

[YouTube channel](https://www.youtube.com/channel/UCcf_3KXjeJSMz39J6gsyTSg)

#### Tuesday 24/03

[![English Video](https://img.youtube.com/vi/TJQ1DIsP8ls/0.jpg)](https://www.youtube.com/watch?v=TJQ1DIsP8ls)

#### Monday 23/03

[![French Video](https://img.youtube.com/vi/MW8I0W8OaoM/0.jpg)](https://www.youtube.com/watch?v=MW8I0W8OaoM)

#### Sunday 22/03

[English Video](https://www.youtube.com/watch?v=GLX5o_y0Vx0)

[French Video, quick](https://www.youtube.com/watch?v=GqGbh__ymw4)

[French Video, long](https://www.youtube.com/watch?v=KqBkgTWbIlg)

# Contact Us

Please [open an issue on GitHub](https://github.com/covid-response-projects/covid-respirator/issues/new) to contact us.

# Renders

## Part 1: The "Air Transistor"

🎦 View: [Animation of the "Air Transistor"](./src/components/air-transistor/schemes/Air%20Transistor%20(Animation%201).mp4)

![Air Transistor Concept](./src/components/air-transistor/schemes/Air%20Transistor%20(Render%201).png)

## Part 2: The "Blower"

🎦 View: [Animation of the "Blower"](./src/components/Blower/schemes/Blower%20(Animation%201).mp4)

![Blower Concept](./src/components/Blower/schemes/Blower%20(Render%201).png)

## Part 3: The "Oxygen Mixer"

![Oxygen Mixer Concept](./src/components/Oxygen-Mixer/schemes/Oxygen%20Mixer%20(Render%201).png)
