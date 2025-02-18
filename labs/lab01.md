---
title: |
  CITS3007 lab 1&nbsp;--&nbsp;Development environment
include-before: |
  ```{=html}
  <style>
    svg p {
      margin: 0;
      padding: 0;
    }

    #svg-container {
      display: none;
    }
    #graph-div-sml {
      width: 30%;
      margin: 0 auto;           /* Center it horizontally */
      display: block;           /* Ensure it's treated as a block element */
    }
    #graph-div {
      width: 70%;
      margin: 0 auto;           /* Center it horizontally */
      display: block;           /* Ensure it's treated as a block element */
    }


  </style>      
  <script>
    let flowchartIsExpanded = false;
  
    function toggleSVG() {
      const container = document.getElementById('svg-container');
      const container_sml = document.getElementById('svg-container-sml');
      console.log("is expanded?", flowchartIsExpanded);

      if (flowchartIsExpanded) {
        container_sml.style.display = 'block';
        container.style.display = 'none';

      } else {
        container_sml.style.display = 'none';
        container.style.display = 'block';
      }
      
      flowchartIsExpanded = !flowchartIsExpanded;
    }

    function stopPropagation(event) {
      event.stopPropagation();
    }
  </script>

  ```    

---

<div style="border: solid 2pt orange; border-radius: 5pt; background-color: hsl(22.35, 100%, 85%, 1); padding: 1em;">

::: block-caption

Laptop and SDE requirement

:::

When attending lab classes, you will need access to a laptop from which you can access the
[CITS3007 standard development environment][faq-dev-env] (SDE), which is based on Ubuntu 20.04,
running on version 5.4.0 of the Linux kernel on an x86-64 processor.

[faq-dev-env]: https://cits3007.arranstewart.io/faq/#cits3007-sde

The CITS3007 SDE will be used for the unit project, later in semester: all
submissions are expected to compile and run correctly in this environment (though you may
find it useful to test your code in other environments, as well).

We may also refer to the SDE in tests or quizzes -- you might be asked to write code which
will compile in this environment, for instance.

In the SDE, you are able to run commands as `root` (by using the [`sudo` command][sudo]), and to
alter the parameters of the running kernel (using the [`sysctl` command][sysctl]). Being able to run
`sudo` is important for many of the labs, so you'll want to make sure it functions correctly
for you. Being able to run the `sysctl` command is of lesser importance -- you will likely
only need to use it if you want to complete the extension tasks for lab 4 (on buffer
overflows) and lab 8 (on race conditions).

[sudo]: https://linux.die.net/man/8/sudo
[sysctl]: https://linux.die.net/man/8/sysctl

The preferred way of accessing the SDE is by running [VirtualBox][virtualbox] and [Vagrant][vagrant]
on your laptop, as outlined in this lab sheet.  (Vagrant is widely used in industry to provide sandard development
environments.)
However, if you run into difficulties, there are a few other options,
outlined below.

Note that UWA provides [financial support][fin-supp] via the "SOS IT Equipment Scheme" to students who
are unable to purchase a laptop due to financial hardship.

[fin-supp]: https://www.uwa.edu.au/students/Support-services/Financial-assistance#:~:text=SOS%20IT%20Equipment%20Scheme

</div>

## 1. Setting up the C development environment { #c-devel-env }

**tl;dr**: Click on the box below to see a flowchart to guide you through the process.


<div id="svg-container-sml">
<svg onclick="toggleSVG()" aria-roledescription="flowchart-v2" role="graphics-document document" viewBox="0 0 246 94" style="max-width: 100%;" class="flowchart" xmlns="http://www.w3.org/2000/svg" width="100%" id="graph-div-sml" height="100%" xmlns:xlink="http://www.w3.org/1999/xlink"><style>#graph-div-sml{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;fill:#333;}#graph-div-sml .error-icon{fill:#552222;}#graph-div-sml .error-text{fill:#552222;stroke:#552222;}#graph-div-sml .edge-thickness-normal{stroke-width:1px;}#graph-div-sml .edge-thickness-thick{stroke-width:3.5px;}#graph-div-sml .edge-pattern-solid{stroke-dasharray:0;}#graph-div-sml .edge-thickness-invisible{stroke-width:0;fill:none;}#graph-div-sml .edge-pattern-dashed{stroke-dasharray:3;}#graph-div-sml .edge-pattern-dotted{stroke-dasharray:2;}#graph-div-sml .marker{fill:#333333;stroke:#333333;}#graph-div-sml .marker.cross{stroke:#333333;}#graph-div-sml svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;}#graph-div-sml p{margin:0;}#graph-div-sml .label{font-family:"trebuchet ms",verdana,arial,sans-serif;color:#333;}#graph-div-sml .cluster-label text{fill:#333;}#graph-div-sml .cluster-label span{color:#333;}#graph-div-sml .cluster-label span p{background-color:transparent;}#graph-div-sml .label text,#graph-div-sml span{fill:#333;color:#333;}#graph-div-sml .node rect,#graph-div-sml .node circle,#graph-div-sml .node ellipse,#graph-div-sml .node polygon,#graph-div-sml .node path{fill:#ECECFF;stroke:#9370DB;stroke-width:1px;}#graph-div-sml .rough-node .label text,#graph-div-sml .node .label text,#graph-div-sml .image-shape .label,#graph-div-sml .icon-shape .label{text-anchor:middle;}#graph-div-sml .node .katex path{fill:#000;stroke:#000;stroke-width:1px;}#graph-div-sml .rough-node .label,#graph-div-sml .node .label,#graph-div-sml .image-shape .label,#graph-div-sml .icon-shape .label{text-align:center;}#graph-div-sml .node.clickable{cursor:pointer;}#graph-div-sml .root .anchor path{fill:#333333!important;stroke-width:0;stroke:#333333;}#graph-div-sml .arrowheadPath{fill:#333333;}#graph-div-sml .edgePath .path{stroke:#333333;stroke-width:2.0px;}#graph-div-sml .flowchart-link{stroke:#333333;fill:none;}#graph-div-sml .edgeLabel{background-color:rgba(232,232,232, 0.8);text-align:center;}#graph-div-sml .edgeLabel p{background-color:rgba(232,232,232, 0.8);}#graph-div-sml .edgeLabel rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#graph-div-sml .labelBkg{background-color:rgba(232, 232, 232, 0.5);}#graph-div-sml .cluster rect{fill:#ffffde;stroke:#aaaa33;stroke-width:1px;}#graph-div-sml .cluster text{fill:#333;}#graph-div-sml .cluster span{color:#333;}#graph-div-sml div.mermaidTooltip{position:absolute;text-align:center;max-width:200px;padding:2px;font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:12px;background:hsl(80, 100%, 96.2745098039%);border:1px solid #aaaa33;border-radius:2px;pointer-events:none;z-index:100;}#graph-div-sml .flowchartTitleText{text-anchor:middle;font-size:18px;fill:#333;}#graph-div-sml rect.text{fill:none;stroke-width:0;}#graph-div-sml .icon-shape,#graph-div-sml .image-shape{background-color:rgba(232,232,232, 0.8);text-align:center;}#graph-div-sml .icon-shape p,#graph-div-sml .image-shape p{background-color:rgba(232,232,232, 0.8);padding:2px;}#graph-div-sml .icon-shape rect,#graph-div-sml .image-shape rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#graph-div-sml :root{--mermaid-font-family:"trebuchet ms",verdana,arial,sans-serif;}</style><g><marker orient="auto" markerHeight="8" markerWidth="8" markerUnits="userSpaceOnUse" refY="5" refX="5" viewBox="0 0 10 10" class="marker flowchart-v2" id="graph-div-sml_flowchart-v2-pointEnd"><path style="stroke-width: 1; stroke-dasharray: 1, 0;" class="arrowMarkerPath" d="M 0 0 L 10 5 L 0 10 z"></path></marker><marker orient="auto" markerHeight="8" markerWidth="8" markerUnits="userSpaceOnUse" refY="5" refX="4.5" viewBox="0 0 10 10" class="marker flowchart-v2" id="graph-div-sml_flowchart-v2-pointStart"><path style="stroke-width: 1; stroke-dasharray: 1, 0;" class="arrowMarkerPath" d="M 0 5 L 10 10 L 10 0 z"></path></marker><marker orient="auto" markerHeight="11" markerWidth="11" markerUnits="userSpaceOnUse" refY="5" refX="11" viewBox="0 0 10 10" class="marker flowchart-v2" id="graph-div-sml_flowchart-v2-circleEnd"><circle style="stroke-width: 1; stroke-dasharray: 1, 0;" class="arrowMarkerPath" r="5" cy="5" cx="5"></circle></marker><marker orient="auto" markerHeight="11" markerWidth="11" markerUnits="userSpaceOnUse" refY="5" refX="-1" viewBox="0 0 10 10" class="marker flowchart-v2" id="graph-div-sml_flowchart-v2-circleStart"><circle style="stroke-width: 1; stroke-dasharray: 1, 0;" class="arrowMarkerPath" r="5" cy="5" cx="5"></circle></marker><marker orient="auto" markerHeight="11" markerWidth="11" markerUnits="userSpaceOnUse" refY="5.2" refX="12" viewBox="0 0 11 11" class="marker cross flowchart-v2" id="graph-div-sml_flowchart-v2-crossEnd"><path style="stroke-width: 2; stroke-dasharray: 1, 0;" class="arrowMarkerPath" d="M 1,1 l 9,9 M 10,1 l -9,9"></path></marker><marker orient="auto" markerHeight="11" markerWidth="11" markerUnits="userSpaceOnUse" refY="5.2" refX="-1" viewBox="0 0 11 11" class="marker cross flowchart-v2" id="graph-div-sml_flowchart-v2-crossStart"><path style="stroke-width: 2; stroke-dasharray: 1, 0;" class="arrowMarkerPath" d="M 1,1 l 9,9 M 10,1 l -9,9"></path></marker><g class="root"><g class="clusters"></g><g class="edgePaths"></g><g class="edgeLabels"></g><g class="nodes"><g transform="translate(123, 47)" id="flowchart-St-9531" class="node default"><rect height="78" width="230" y="-39" x="-115" ry="5" rx="5" style="" class="basic label-container"></rect><g transform="translate(-100, -24)" style="" class="label"><rect></rect><foreignObject height="48" width="200"><div style="display: table;  line-height: 1.1; max-width: 200px; text-align: center; width: 200px;" xmlns="http://www.w3.org/1999/xhtml"><span class="nodeLabel"><p>click to expand decision tree</p></span></div></foreignObject></g></g></g></g></g></svg>
</div>

<div id="svg-container">
<svg onclick="toggleSVG()" aria-roledescription="flowchart-v2" role="graphics-document document" viewBox="0 0 724.5 1302" style="max-width: 100%;" class="flowchart" xmlns="http://www.w3.org/2000/svg" width="100%" id="graph-div" height="100%" xmlns:xlink="http://www.w3.org/1999/xlink"><style>#graph-div{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;fill:#333;}#graph-div .error-icon{fill:#552222;}#graph-div .error-text{fill:#552222;stroke:#552222;}#graph-div .edge-thickness-normal{stroke-width:1px;}#graph-div .edge-thickness-thick{stroke-width:3.5px;}#graph-div .edge-pattern-solid{stroke-dasharray:0;}#graph-div .edge-thickness-invisible{stroke-width:0;fill:none;}#graph-div .edge-pattern-dashed{stroke-dasharray:3;}#graph-div .edge-pattern-dotted{stroke-dasharray:2;}#graph-div .marker{fill:#333333;stroke:#333333;}#graph-div .marker.cross{stroke:#333333;}#graph-div svg{font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:16px;}#graph-div p{margin:0;}#graph-div .label{font-family:"trebuchet ms",verdana,arial,sans-serif;color:#333;}#graph-div .cluster-label text{fill:#333;}#graph-div .cluster-label span{color:#333;}#graph-div .cluster-label span p{background-color:transparent;}#graph-div .label text,#graph-div span{fill:#333;color:#333;}#graph-div .node rect,#graph-div .node circle,#graph-div .node ellipse,#graph-div .node polygon,#graph-div .node path{fill:#ECECFF;stroke:#9370DB;stroke-width:1px;}#graph-div .rough-node .label text,#graph-div .node .label text,#graph-div .image-shape .label,#graph-div .icon-shape .label{text-anchor:middle;}#graph-div .node .katex path{fill:#000;stroke:#000;stroke-width:1px;}#graph-div .rough-node .label,#graph-div .node .label,#graph-div .image-shape .label,#graph-div .icon-shape .label{text-align:center;}#graph-div .node.clickable{cursor:pointer;}#graph-div .root .anchor path{fill:#333333!important;stroke-width:0;stroke:#333333;}#graph-div .arrowheadPath{fill:#333333;}#graph-div .edgePath .path{stroke:#333333;stroke-width:2.0px;}#graph-div .flowchart-link{stroke:#333333;fill:none;}#graph-div .edgeLabel{background-color:rgba(232,232,232, 0.8);text-align:center;}#graph-div .edgeLabel p{background-color:rgba(232,232,232, 0.8);}#graph-div .edgeLabel rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#graph-div .labelBkg{background-color:rgba(232, 232, 232, 0.5);}#graph-div .cluster rect{fill:#ffffde;stroke:#aaaa33;stroke-width:1px;}#graph-div .cluster text{fill:#333;}#graph-div .cluster span{color:#333;}#graph-div div.mermaidTooltip{position:absolute;text-align:center;max-width:200px;padding:2px;font-family:"trebuchet ms",verdana,arial,sans-serif;font-size:12px;background:hsl(80, 100%, 96.2745098039%);border:1px solid #aaaa33;border-radius:2px;pointer-events:none;z-index:100;}#graph-div .flowchartTitleText{text-anchor:middle;font-size:18px;fill:#333;}#graph-div rect.text{fill:none;stroke-width:0;}#graph-div .icon-shape,#graph-div .image-shape{background-color:rgba(232,232,232, 0.8);text-align:center;}#graph-div .icon-shape p,#graph-div .image-shape p{background-color:rgba(232,232,232, 0.8);padding:2px;}#graph-div .icon-shape rect,#graph-div .image-shape rect{opacity:0.5;background-color:rgba(232,232,232, 0.8);fill:rgba(232,232,232, 0.8);}#graph-div :root{--mermaid-font-family:"trebuchet ms",verdana,arial,sans-serif;}</style><g><marker orient="auto" markerHeight="8" markerWidth="8" markerUnits="userSpaceOnUse" refY="5" refX="5" viewBox="0 0 10 10" class="marker flowchart-v2" id="graph-div_flowchart-v2-pointEnd"><path style="stroke-width: 1; stroke-dasharray: 1, 0;" class="arrowMarkerPath" d="M 0 0 L 10 5 L 0 10 z"></path></marker><marker orient="auto" markerHeight="8" markerWidth="8" markerUnits="userSpaceOnUse" refY="5" refX="4.5" viewBox="0 0 10 10" class="marker flowchart-v2" id="graph-div_flowchart-v2-pointStart"><path style="stroke-width: 1; stroke-dasharray: 1, 0;" class="arrowMarkerPath" d="M 0 5 L 10 10 L 10 0 z"></path></marker><marker orient="auto" markerHeight="11" markerWidth="11" markerUnits="userSpaceOnUse" refY="5" refX="11" viewBox="0 0 10 10" class="marker flowchart-v2" id="graph-div_flowchart-v2-circleEnd"><circle style="stroke-width: 1; stroke-dasharray: 1, 0;" class="arrowMarkerPath" r="5" cy="5" cx="5"></circle></marker><marker orient="auto" markerHeight="11" markerWidth="11" markerUnits="userSpaceOnUse" refY="5" refX="-1" viewBox="0 0 10 10" class="marker flowchart-v2" id="graph-div_flowchart-v2-circleStart"><circle style="stroke-width: 1; stroke-dasharray: 1, 0;" class="arrowMarkerPath" r="5" cy="5" cx="5"></circle></marker><marker orient="auto" markerHeight="11" markerWidth="11" markerUnits="userSpaceOnUse" refY="5.2" refX="12" viewBox="0 0 11 11" class="marker cross flowchart-v2" id="graph-div_flowchart-v2-crossEnd"><path style="stroke-width: 2; stroke-dasharray: 1, 0;" class="arrowMarkerPath" d="M 1,1 l 9,9 M 10,1 l -9,9"></path></marker><marker orient="auto" markerHeight="11" markerWidth="11" markerUnits="userSpaceOnUse" refY="5.2" refX="-1" viewBox="0 0 11 11" class="marker cross flowchart-v2" id="graph-div_flowchart-v2-crossStart"><path style="stroke-width: 2; stroke-dasharray: 1, 0;" class="arrowMarkerPath" d="M 1,1 l 9,9 M 10,1 l -9,9"></path></marker><g class="root"><g class="clusters"></g><g class="edgePaths"><path marker-end="url(#graph-div_flowchart-v2-pointEnd)" style="" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" id="L_St_Prob_0" d="M275,62L275,66.167C275,70.333,275,78.667,275.07,86.417C275.141,94.167,275.281,101.334,275.351,104.917L275.422,108.501"></path><path marker-end="url(#graph-div_flowchart-v2-pointEnd)" style="" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" id="L_Prob_NoAttempt_1" d="M207.994,346.994L193.828,364.328C179.663,381.663,151.331,416.331,137.166,453.832C123,491.333,123,531.667,123,551.833L123,572"></path><path marker-end="url(#graph-div_flowchart-v2-pointEnd)" style="" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" id="L_Prob_Win_2" d="M343.006,346.994L357.005,364.328C371.004,381.663,399.002,416.331,413.075,439.249C427.149,462.167,427.298,473.334,427.372,478.917L427.447,484.5"></path><path marker-end="url(#graph-div_flowchart-v2-pointEnd)" style="" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" id="L_Win_MacSil_3" d="M361.418,700.418L345.765,717.515C330.112,734.612,298.806,768.806,283.227,791.486C267.649,814.167,267.798,825.334,267.872,830.917L267.947,836.5"></path><path marker-end="url(#graph-div_flowchart-v2-pointEnd)" style="" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" id="L_Win_WinYes_4" d="M493.582,700.418L509.068,717.515C524.555,734.612,555.527,768.806,571.014,808.07C586.5,847.333,586.5,891.667,586.5,913.833L586.5,936"></path><path marker-end="url(#graph-div_flowchart-v2-pointEnd)" style="" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" id="L_MacSil_Issues_5" d="M206.418,1056.918L193.265,1073.265C180.112,1089.612,153.806,1122.306,140.653,1144.153C127.5,1166,127.5,1177,127.5,1182.5L127.5,1188"></path><path marker-end="url(#graph-div_flowchart-v2-pointEnd)" style="" class="edge-thickness-normal edge-pattern-solid edge-thickness-normal edge-pattern-solid flowchart-link" id="L_MacSil_MacSilYes_6" d="M329.582,1056.918L342.569,1073.265C355.555,1089.612,381.527,1122.306,394.514,1144.153C407.5,1166,407.5,1177,407.5,1182.5L407.5,1188"></path></g><g class="edgeLabels"><g class="edgeLabel"><g transform="translate(0, 0)" class="label"><foreignObject height="0" width="0"><div style="display: table-cell; white-space: nowrap; line-height: 1.1; max-width: 200px; text-align: center;" class="labelBkg" xmlns="http://www.w3.org/1999/xhtml"><span class="edgeLabel"></span></div></foreignObject></g></g><g transform="translate(123, 451)" class="edgeLabel"><g transform="translate(-9.3984375, -12)" class="label"><foreignObject height="24" width="18.796875"><div style="display: table-cell; white-space: nowrap; line-height: 1.1; max-width: 200px; text-align: center;" class="labelBkg" xmlns="http://www.w3.org/1999/xhtml"><span class="edgeLabel"><p>No</p></span></div></foreignObject></g></g><g transform="translate(427, 451)" class="edgeLabel"><g transform="translate(-11.328125, -12)" class="label"><foreignObject height="24" width="22.65625"><div style="display: table-cell; white-space: nowrap; line-height: 1.1; max-width: 200px; text-align: center;" class="labelBkg" xmlns="http://www.w3.org/1999/xhtml"><span class="edgeLabel"><p>Yes</p></span></div></foreignObject></g></g><g transform="translate(267.5, 803)" class="edgeLabel"><g transform="translate(-9.3984375, -12)" class="label"><foreignObject height="24" width="18.796875"><div style="display: table-cell; white-space: nowrap; line-height: 1.1; max-width: 200px; text-align: center;" class="labelBkg" xmlns="http://www.w3.org/1999/xhtml"><span class="edgeLabel"><p>No</p></span></div></foreignObject></g></g><g transform="translate(586.5, 803)" class="edgeLabel"><g transform="translate(-11.328125, -12)" class="label"><foreignObject height="24" width="22.65625"><div style="display: table-cell; white-space: nowrap; line-height: 1.1; max-width: 200px; text-align: center;" class="labelBkg" xmlns="http://www.w3.org/1999/xhtml"><span class="edgeLabel"><p>Yes</p></span></div></foreignObject></g></g><g transform="translate(127.5, 1155)" class="edgeLabel"><g transform="translate(-9.3984375, -12)" class="label"><foreignObject height="24" width="18.796875"><div style="display: table-cell; white-space: nowrap; line-height: 1.1; max-width: 200px; text-align: center;" class="labelBkg" xmlns="http://www.w3.org/1999/xhtml"><span class="edgeLabel"><p>No</p></span></div></foreignObject></g></g><g transform="translate(407.5, 1155)" class="edgeLabel"><g transform="translate(-11.328125, -12)" class="label"><foreignObject height="24" width="22.65625"><div style="display: table-cell; white-space: nowrap; line-height: 1.1; max-width: 200px; text-align: center;" class="labelBkg" xmlns="http://www.w3.org/1999/xhtml"><span class="edgeLabel"><p>Yes</p></span></div></foreignObject></g></g></g><g class="nodes"><g transform="translate(275, 35)" id="flowchart-St-9546" class="node default"><rect height="54" width="101.03125" y="-27" x="-50.515625" ry="5" rx="5" style="" class="basic label-container"></rect><g transform="translate(-35.515625, -12)" style="" class="label"><rect></rect><foreignObject height="24" width="71.03125"><div style="display: table-cell; white-space: nowrap; line-height: 1.1; max-width: 200px; text-align: center;" xmlns="http://www.w3.org/1999/xhtml"><span class="nodeLabel"><p>start here</p></span></div></foreignObject></g></g><g transform="translate(275, 263)" id="flowchart-Prob-9547" class="node default"><polygon transform="translate(-151,151)" class="label-container" points="151,0 302,-151 151,-302 0,-151"></polygon><g transform="translate(-100, -36)" style="" class="label"><rect></rect><foreignObject height="72" width="200"><div style="display: table;  line-height: 1.1; max-width: 200px; text-align: center; width: 200px;" xmlns="http://www.w3.org/1999/xhtml"><span class="nodeLabel"><p>Have you tried installing Vagrant and Virtualbox, but run into problems?</p></span></div></foreignObject></g></g><g transform="translate(123, 627)" id="flowchart-NoAttempt-9549" class="node default"><rect height="102" width="230" y="-51" x="-115" ry="5" rx="5" style="" class="basic label-container"></rect><g transform="translate(-100, -36)" style="" class="label"><rect></rect><foreignObject height="72" width="200"><div style="display: table;  line-height: 1.1; max-width: 200px; text-align: center; width: 200px;" xmlns="http://www.w3.org/1999/xhtml"><span class="nodeLabel"><p>Follow <a href="#virtualbox-and-vagrant" onclick="stopPropagation(event)">the instructions</a> for installing Vagrant and VirtualBox</p></span></div></foreignObject></g></g><g transform="translate(427, 627)" id="flowchart-Win-9551" class="node default"><polygon transform="translate(-139,139)" class="label-container" points="139,0 278,-139 139,-278 0,-139"></polygon><g transform="translate(-100, -24)" style="" class="label"><rect></rect><foreignObject height="48" width="200"><div style="display: table;  line-height: 1.1; max-width: 200px; text-align: center; width: 200px;" xmlns="http://www.w3.org/1999/xhtml"><span class="nodeLabel"><p>Are you using a recent version of Windows?</p></span></div></foreignObject></g></g><g transform="translate(267.5, 979)" id="flowchart-MacSil-9553" class="node default"><polygon transform="translate(-139,139)" class="label-container" points="139,0 278,-139 139,-278 0,-139"></polygon><g transform="translate(-100, -24)" style="" class="label"><rect></rect><foreignObject height="48" width="200"><div style="display: table;  line-height: 1.1; max-width: 200px; text-align: center; width: 200px;" xmlns="http://www.w3.org/1999/xhtml"><span class="nodeLabel"><p>Are you using a Silicon-based Mac?</p></span></div></foreignObject></g></g><g transform="translate(586.5, 979)" id="flowchart-WinYes-9555" class="node default"><rect height="118" width="260" y="-39" x="-130" style="" class="basic label-container"></rect><g transform="translate(-100, -24)" style="" class="label"><rect></rect><foreignObject height="108" width="200"><div style="display: table;  line-height: 1.1; max-width: 200px; text-align: center; width: 200px;" xmlns="http://www.w3.org/1999/xhtml"><span class="nodeLabel"><p>Consider using <a href="#use-windows-wsl" onclick="stopPropagation(event)">Windows WSL</a> for most weeks, work with another student for Lab 5</p></span></div></foreignObject></g></g><g transform="translate(127.5, 1243)" id="flowchart-Issues-9557" class="node default"><rect height="102" width="230" y="-51" x="-115" ry="5" rx="5" style="" class="basic label-container"></rect><g transform="translate(-100, -36)" style="" class="label"><rect></rect><foreignObject height="72" width="200"><div style="display: table;  line-height: 1.1; max-width: 200px; text-align: center; width: 200px;" xmlns="http://www.w3.org/1999/xhtml"><span class="nodeLabel"><p>Discuss with lab facilitator; <a href="#use-gitpod" onclick="stopPropagation(event)">use GitPod</a> in the meantime</p></span></div></foreignObject></g></g><g transform="translate(407.5, 1243)" id="flowchart-MacSilYes-9559" class="node default"><rect height="102" width="230" y="-51" x="-115" ry="5" rx="5" style="" class="basic label-container"></rect><g transform="translate(-100, -36)" style="" class="label"><rect></rect><foreignObject height="72" width="200"><div style="display: table;  line-height: 1.1; max-width: 200px; text-align: center; width: 200px;" xmlns="http://www.w3.org/1999/xhtml"><span class="nodeLabel"><p>You won't be able to use VirtualBox; <a href="#use-utm" onclick="stopPropagation(event)">use UTM</a> for MacOS instead</p></span></div></foreignObject></g></g></g></g></g></svg>
</div>



To access the CITS3007 standard development environment, the preferred option is:

<div style="margin-left: 1rem; border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

**Use Virtualbox and Vagrant**

:   Install two open source tools ([VirtualBox][virtualbox] and
    [Vagrant][vagrant]) on your Windows or Linux laptop. See
    [VirtualBox and Vagrant](#virtualbox-and-vagrant), below.

    You will need about 15 GB of disk space and a minimum of 4 GB of RAM on your
    laptop. (Note that 4 GB of RAM is likely to result in your VM
    running very slowly -- 8 GB or more is preferable.)

</div>

However, VirtualBox is not available for M-series (Apple Silicon) mac laptops -- in that case, you
might like to try:

<div id="use-utm" style="margin-left: 1rem; border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

**Use UTM for Mac**

:   This involves installing UTM, virtualization software for the macOS operating system.
    See [Using UTM on M-series Mac laptops](lab01-utm.html).
    As explained on that page, you'll need to pick one or more Linux virtual machine images.
    If compiling code for the project or the lab 4 and 8 extension tasks, you'll need
    to make sure you are using an appropriate virtual machine.

    In general, the facilitators and unit coordinator won't be able to assist you if you run
    into problems using UTM, but you should feel free to post on the Help3007 discussion
    forum, as other students using Macs may be able to assist.

</div>

Virtualbox also may not function properly on Windows laptops where you are already using
the [Windows subsystem for Linux][wsl] (WSL).

[wsl]: https://learn.microsoft.com/en-us/windows/wsl/about

In that case, you might wish to:

<div id="use-windows-wsl" style="margin-left: 1rem; border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

**Use Ubuntu 20.04 from WSL on Windows**

:   In this case, we assume you are already familiar with how to install
    and access Ubuntu distributions using the WSL.

    In general, the facilitators and unit coordinator won't be able to assist you if you run
    into problems using the WSL, but you can find general instructions on using it
    [here][using-wsl].

    If you use the WSL, you *will* be able to run commands as root using `sudo`, but may
    not be able to alter kernel parameters using `sysctl`.

</div>

[using-wsl]: https://learn.microsoft.com/en-us/windows/wsl/install

If none of the above options work for you, a remaining option is:

<div id="use-gitpod" style="margin-left: 1rem; border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

**Access the SDE using GitPod**

:   [GitPod][gitpod]
    provides web-based access to development environments
    hosted in the cloud, and provides a quota of free hours each month (which most students
    will probably not exceed).

    We provide instructions on how to use GitPod [here](lab01-gitpod.html).

    If using GitPod, you *will* be able to run commands as root using `sudo`, but won't
    be able to alter kernel parameters using `sysctl`.

</div>

[virtualbox]: https://www.virtualbox.org
[vagrant]: https://www.vagrantup.com
[gitpod]: https://gitpod.io/




### VirtualBox and Vagrant { #virtualbox-and-vagrant }

<div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">

::: block-caption

What are VirtualBox and Vagrant?

:::

**VirtualBox** is a type of [virtualization software][virtualization] which allows you to
run other operating systems on your computer – even operating systems designed for
completely different hardware (such as mobile phones, for instance).  It allows precise
control of what OS [kernel][kernel] is run.
(Other technologies for running Linux do exist -- for instance, Docker and Windows
Subsystem for Linux, or WSL -- but they do not allow the precise control over kernel version
that VirtualBox does.)

**Vagrant** is a tool for managing virtual environments. It can manage environments created
using VirtualBox, Docker, VMWare, WSL, and many more.  It handles tasks such as

- fetching virtual machine (VM) [images][vm-image] from the web and installing them in a
  standard location;
- checking whether any updates have been issued for an image, and prompting the user to
  download them, if so;
- allowing a user to easily manage multiple versions of the same basic VM image; and
- abstracting over the details of particular virtualization technologies.

It's widely used in industry to provide developers with a Standard Development Environment
(SDE) for their team or organization, so it's hoped that experience with Vagrant will prove
useful beyond your university studies.

Since the purpose of Vagrant is to manage VMs on a computer, vagrant commands need to be run
from the *host* computer (for instance, your laptop), not from within a VM.

[virtualization]: https://en.wikipedia.org/w/index.php?title=Hardware_virtualization
[kernel]: https://en.wikipedia.org/wiki/Kernel_(operating_system)
[vm-image]: https://en.wikipedia.org/w/index.php?title=Disk_image

</div>



To install VirtualBox and Vagrant:

1.  Visit <https://www.virtualbox.org/wiki/Downloads>, download
    the appropriate VirtualBox package for your platform (Windows, MacOS or Linux),
    and install it.

    Note, if you are using a Mac: VirtualBox is only available for Macs using **x86-64
    processors**. If you are using a more recent M-series (Apple Silicon) Mac, then it uses an ARM64
    processor, and you should be using UTM, as described previously.
    If you don't know which yours is: you can go to the 'Apple' at the
    top left of your screen and select 'About this Mac', and it will show whether your
    cpu is Apple silicon (ARM, M1 or M2) or x86/Intel.

    - On Windows, the installer is an `.exe` file you can simply run
      by double-clicking.
    - On MacOS, the installer is a `.dmg` file -- double click on it
      for instructions.
    - On Ubuntu distributions a `.deb` file is provided -- we assume you already know how to install
      one of these. If you are using a non-Ubuntu distribution, consult
      the VirtualBox documentation for instructions on how to install.

    If on Linux, once VirtualBox is installed, you should run the command `sudo usermod -aG
    vboxusers $USER` from a terminal; this gives you all the permissions needed to run
    VirtualBox properly. You'll then need to log out and log back in again.

#.  Visit <https://www.vagrantup.com/downloads>, download the
    appropriate Vagrant package for your platform, and install it.

    - On Windows, the installer is an `.msi` file you can run by
      double-clicking.
    - On MacOS, you can either download a `.dmg` file, or
      (if you have [Homebrew](https://brew.sh) installed) there are
      instructions for adding Vagrant with a `brew install` command.
    - On Ubuntu, it's recommended you install Vagrant using the APT package
      manager (see "Package manager for Linux" on the Vagrant page).

    <!--
    macos old intel: may hit network issue, just google and amend
    vagrantfile
    -->

#.  Check that Vagrant has been successfully installed.

    On Windows, open a "Command Prompt" window. On MacOS or Linux, open
    a terminal window.

    Then type `vagrant --version`; Vagrant should display

    ```
    Vagrant 2.3.7
    ```

    (Or something similar – any version from 2.2.0 onwards should be
    fine.)

#.  Test a small virtual machine (VM) image.

    On your *host machine* (i.e. your laptop), create a new directory
    (called e.g. `vagrant-test`), open a terminal window and `cd` into
    the directory. (The `cd` command works on Linux, Mac and Windows.)

    **Download a test VM image**. Run the command `vagrant box add --provider=virtualbox generic/alpine319`.
    Vagrant will then attempt to download a virtual machine image containing [Alpine Linux][alpine] from
    `https://vagrantcloud.com/`, a central repository of VM images.

    It may take about a minute to download the image over WiFi; if successful, it should
    display something similar to the following:

    ```
    ==> box: Loading metadata for box 'generic/alpine319'
        box: URL: https://vagrantcloud.com/api/v2/vagrant/generic/alpine319
    ==> box: Adding box 'generic/alpine319' (v4.3.12) for provider: virtualbox (amd64)
        box: Downloading: https://vagrantcloud.com/generic/boxes/alpine319/versions/4.3.12/providers/virtualbox/amd64/vagrant.box
        box: Calculating and comparing box checksum...
    ==> box: Successfully added box 'generic/alpine319' (v4.3.12) for 'virtualbox (amd64)'!
    ```

    **Create a Vagrantfile**. Assuming this step was successful, run the command `vagrant init generic/alpine319`.
    This creates a file named `Vagrantfile`, which can be used to configure a VM before you
    run it (for instance, by altering the amount of RAM allocated to the VM, or the network
    configuration). You can take a look at the file contents in any text editor.

    **Bring up the VM**. Run `vagrant up --provider=virtualbox`. Vagrant will then attempt
    to configure the VM for use and start it running.

    <!--
    troubleshooting tips:

    try editing the Vagrantfile and un-commenting the "gui = true" section:
    then you can get gui access to the VM; use username and password "vagrant"
    to log in.

    if on linux: ensure you follow the instruction for installing via the package manager
    for your distro (see <https://developer.hashicorp.com/vagrant/install#linux>), rather
    than just downloading the binary.

    -->


    If you type `vagrant ssh`, Vagrant will connect to the new virtual
    machine using [`ssh`][ssh] and present you with a shell prompt:

    ```
    $ vagrant ssh
    alpine319:~$
    ```

    Note that the [command-line prompt][prompt] has now **changed**
    from the usual terminal prompt you see. The prompt `alpine319:~$`
    indicates that you're running commands in an Alpine Linux VM. To be able
    to run commands from your host machine again, you need to exit
    from your SSH session to the VM. (Or alternatively, of course, you
    can simply open a new terminal window.)

    You can exit your current SSH session by typing `exit` or
    hitting `ctrl-D`.

    (It's not strictly necessary, but it's good
    practice to also *halt* VMs when you're not using them --
    you can do so by issuing the command `vagrant halt` after you've
    exited the SSH session.)

    <div style="border: solid 2pt orange; border-radius: 5pt; background-color: hsl(22.35, 100%, 85%, 1); padding: 1em;">

    ::: block-caption

    Vagrant username and password

    :::

    Normally, you shouldn't need a username and password to access a Vagrant-managed virtual
    machine at all – running `vagrant ssh` should log you into the machine without any other
    authentication being needed.

    Should you ever need to specify a username and password, though, the CITS3007 SDE
    follows the standard Vagrant convention of setting them both to "`vagrant`".

    </div>


    <div style="border: solid 2pt orange; border-radius: 5pt; background-color: hsl(22.35, 100%, 85%, 1); padding: 1em; margin: 1em 0; ">

    ::: block-caption

    Troubleshooting on Linux

    :::

    Note that on Linux, if you happen to already have `libvirt` installed:
    `libvirt` and VirtualBox can't both run at the same time. Ensure
    that `libvirt` isn't running by typing `sudo systemctl stop
    libvirtd`.

    </div>

    [alpine]: https://www.alpinelinux.org
    [ssh]: https://www.ssh.com/academy/ssh
    [prompt]: https://en.wikipedia.org/wiki/Command-line_interface#Command_prompt

#.  Download a VM containing the standard CITS3007 development
    environment. Note that this will likely take **around 5--10
    minutes**, so once you've started the process, move onto the next
    exercise.

    On your *host machine* (i.e. your laptop), create a new directory
    (called e.g. `cits3007-test`), open a terminal window and `cd` into
    the directory.

    **Download the CITS3007 VM image**. Run the command `vagrant box add
    --provider=virtualbox arranstewart/cits3007-ubuntu2004`.
    Vagrant should take around 5--10 minutes
    to download a virtual machine. The output
    from Vagrant should look something like the following:

    ```
    ==> box: Loading metadata for box 'arranstewart/cits3007-ubuntu2004'
        box: URL: https://vagrantcloud.com/api/v2/vagrant/arranstewart/cits3007-ubuntu2004
    ==> box: Adding box 'arranstewart/cits3007-ubuntu2004' (v0.1.4) for provider: virtualbox
        box: Downloading: https://vagrantcloud.com/arranstewart/boxes/cits3007-ubuntu2004/versions/0.1.4/providers/virtualbox/unknown/vagrant.box
        box: Calculating and comparing box checksum...
    ==> box: Successfully added box 'arranstewart/cits3007-ubuntu2004' (v0.1.4) for 'virtualbox'!
    ```

    Type `vagrant init arranstewart/cits3007-ubuntu2004` to create a Vagrantfile, then `vagrant
    up --provider=virtualbox` to start the VM. You should see output like the following:

    ```
    Bringing machine 'default' up with 'virtualbox' provider...
    ==> default: Importing base box 'arranstewart/cits3007-ubuntu2004'...
    ==> default: Matching MAC address for NAT networking...
    ==> default: Checking if box 'arranstewart/cits3007-ubuntu2004' version '0.1.2' is up to date...
    ==> default: Setting the name of the VM: xxx-cits3007-test_1658816964468_16898
    ==> default: Clearing any previously set forwarded ports...
    ==> default: Fixed port collision for 22 => 2222. Now on port 2200.
    ==> default: Clearing any previously set network interfaces...
    ==> default: Preparing network interfaces based on configuration...
        default: Adapter 1: nat
    ==> default: Forwarding ports...
        default: 22 (guest) => 2200 (host) (adapter 1)
    ==> default: Booting VM...
    ==> default: Waiting for machine to boot. This may take a few minutes...
        default: SSH address: 127.0.0.1:2200
        default: SSH username: vagrant
        default: SSH auth method: private key
        default: Warning: Connection reset. Retrying...
        default: Warning: Remote connection disconnect. Retrying...
        default: Warning: Connection reset. Retrying...
        default:
        default: Vagrant insecure key detected. Vagrant will automatically replace
        default: this with a newly generated keypair for better security.
        default:
        default: Inserting generated public key within guest...
        default: Removing insecure key from the guest if it's present...
        default: Key inserted! Disconnecting and reconnecting using new SSH key...
    ==> default: Machine booted and ready!
    ==> default: Checking for guest additions in VM...
    ==> default: Setting hostname...
    ```

    Once it's done, you can type `vagrant ssh` to get shell access to
    the new VM:

    ```
    $ vagrant ssh
    vagrant@cits3007-ubuntu2004:~$
    ```

    Note that again, your terminal prompt has changed -- it sould look
    like this:

    ```
    vagrant@cits3007-ubuntu2004:~$
    ```

    (usually in bright green). The prompt indicates the name of the
    current user ("`vagrant`")[^vagrant-user], and the hostname for the
    machine you are connected to ("`cits3007-ubuntu2004`").

    If you want to issue commands on your host machine again (that is,
    on your laptop), you'll need to either

    - exit your current SSH session, either by running the command `exit`
      or by typing `ctrl-D`
    - open a new terminal window, and issue your commands from there.

    In particular, trying to run Vagrant commands from within your VM
    will *not* work, since Vagrant has not been installed within the VM.

#.  Optional: install VS Code and the "Remote-SSH" extension.

    The standard CITS3007 development environment VM comes with the
    `vim` editor already installed, and for some tasks we do,
    it will be necessary to use `vim` or another terminal-based
    editor.

    However, if you already have VS Code installed and are familiar
    with it, we provide instructions on how to configure it
    for use with a Vagrant box [here](lab01-vs-code.html).

[^vagrant-user]: Conventionally, all Vagrant VMs have a user account
  named "vagrant" on them, with the password for the account also
  set to "vagrant". This makes it very easy to log into them.
  Normally, using a well-known, easy-to-guess, hard-coded user ID and
  password would be a [*poor* security practice][default-creds].\
  &nbsp;
  But in this case: the VM is only being used for development purposes,
  rather than deployed in production; it should only be possible to
  connect to the VM from your local computer; and the VM is unlikely to
  contain any confidential data of yours. This being so, the use of
  default credentials is usually an acceptable risk.

[default-creds]: https://en.wikipedia.org/wiki/Default_Credential_vulnerability

#### Vagrant cheat sheet

You can find a useful "cheat sheet" of commands for managing Vagrant VMs
here:
<https://cheatography.com/davbfr/cheat-sheets/vagrant-cheat-sheet/>.




## 2. Concept review questions

If possible, it's recommended you work on this section in pairs or small
groups and compare answers.

### 2.1. Definitions

For each of the following concepts, review the definition given in the
lecture slides, and explain it in your own words. If you can, give an
example, based on your own experience or from recent news reports:

a. threat
#. vulnerability
#. confidentiality
#. integrity
#. availability







### 2.2. Concepts

a.  How would you describe the relationship between threats,
    vulnerabilities, and attacks? Give examples to illustrate the
    relationship.

    



#.  Is it possible for a *threat* and a *vulnerability* to both exist,
    without there being an attack? Explain why or why not.

    

#.  Consider an ATM, which requires users to provide a bank card and a
    PIN in order to perform transactions. Give examples of
    *confidentiality*, *integrity* and *availability* requirements
    associated with the system. How important do you think each
    requirement is -- are they all equally important, or does the
    importance differ?

    

## 3. AusCERT security bulletins

It's recommended you complete this exercise in pairs or small groups.
Security Bulletins are sometimes not easy to understand, at first
glance, but by sharing ideas and comparing your understanding with
other students, you should be able to complete the questions in this
section.

[AusCERT][auscert] is a non-profit organisation that provides advice on cybersecurity
threats and vulnerabilities. Some of its information is publicly
available, and some is provided only to members.

Visit its website at <http://www.auscert.org.au/>, and from the
"Resources" tab, select "Security Bulletins". (The direct link is
<https://auscert.org.au/bulletins/>.)

Take a look at several, and try searching for
`ASB-2022.0077` and `ESB-2022.3655`. All bulletins have the following
features:

- They have an *ID number*. Bulletins with an ID number
  beginning with "ESB" are bulletins from external sources, being
  redistributed by AusCERT; those beginning with "ASB" are published by
  AusCERT itself. The ID number contains 4 digits indicating what year the
  bulletin was published, followed by the bulletin number within that
  year.
- They specify the product affected by some vulnerability, the operating
  system affected, and what the *resolution* (i.e. action to be taken,
  in order to mitigate or prevent the vulnerability) is.
- They refer to one or more *CVE* identifiers. The CVE ("Common Vulnerabilities and
  Exposures") identification scheme is provided by the US-based MITRE
  organisation. For instance, Auscert's bulletin `ASB-2022.0129`
  refers to `CVE-2022-30168`, the details of which can be found at
  <https://www.cve.org/CVERecord?id=CVE-2022-30168>.
- They give a CVSS (Common Vulnerability Scoring System) score. This is
  a way of quantifying the severity of a vulnerability using a number
  from 0 to 10, where 0 is the least severe and 10 the most.
  Read more about these severity ratings at
  <https://nvd.nist.gov/vuln-metrics/cvss>.

Now, find the `ESB-2022.1671` bulletin and read through it, and refer to
the NIST page on severity ratings at <https://nvd.nist.gov/vuln-metrics/cvss>.
Then answer the following questions:

a.  The bulletin is for a patch which fixes a *vulnerability*.
    What is the vulnerability (give a CVE identifier and a description)?
    In what product does it occur?

    

#.  How severe is the vulnerability? If a vulnerability is severe, does
    that necessarily mean it is easy to exploit, and/or has a high
    probability of being exploited?


    


Locate the CVE information page on <https://www.cve.org> for
this vulnerability, and follow the links there to find out more
about it. Google for the definitions of any terms you do not know --
[Wikipedia](https://en.wikipedia.org/) has good summaries of most terms.
Answer the following questions:

c.  How could an attacker exploit this vulnerability? Describe
    a scenario in which this could occur.

    


#.  Of the "C I A" security goals -- which ones could be compromised
    if this vulnerability were exploited?

    





[auscert]: http://www.auscert.org.au/

## 4. C development

<div style="border: solid 2pt orange; border-radius: 5pt; background-color: hsl(22.35, 100%, 85%, 1); padding: 1em;">

::: block-caption

C language familiarity

:::

Ideally, you should be doing this unit in your third year, and have
familiarity with the C programming language.

You need to either

- have access to a **good C programming textbook** -- see the
  CITS3007 ["Resources"
  page](https://cits3007.arranstewart.io/resources/#c-programming) for one
  recommendation -- or
- be comfortable searching through and interpreting the C language pages at
  **<https://en.cppreference.com/>**.

Note that YouTube tutorials will **not** be sufficient as a C reference
for this unit -- you **will** need a textbook (or to make use of the cppreference.com
site). The lab facilitators will not be able to assist you if you are
using video tutorials instead of a C language reference.

If you are still in the process of purchasing a textbook, the
following sites may provide a helpful refresher:

- Carl Burch, [*C for Python programmers*][c-for-py]
  - Slides based on [*C for Python programmers*][c-for-py-slides]
- Steven Simpson, [*Learning C from Java*][c-from-java]
- Daniel Weller and Sharat Chikkerur, [Practical Programming in C][prac-c] (lecture slides)

[c-for-py]: http://www.cburch.com/books/cpy/index.html
[c-for-py-slides]: https://web.cs.hacettepe.edu.tr/~bbm101/fall16/lectures/w12-c-for-python-programmers.pdf
[c-from-java]: https://ssjools.hopto.org/java2c/alldiffs
[prac-c]: https://ocw.mit.edu/courses/6-087-practical-programming-in-c-january-iap-2010/pages/lecture-notes/

</div>


### 4.1. C questions

a.  What is the difference between the C literal values `3`, `'3'`,
    `"3"` and `3.0`? What is the type of each?

    





#.  Consider the following C code:

    ```C
    const char * str = "test";
    char c = str[0];

    if (c >= 65 && c <= 90) {
      printf("high\n");
    } else if (c >= 97 && c <=122) {
      printf("low\n");
    } else {
      printf("other\n");
    }
    ```

    If this code is executed, what will be printed to the terminal?


    

#.  What is the output of the question (b) code if we change the string
    `"test"` to `"TEST"`?


    

#.  What is the output of the question (b) code if we change the string
    `"test"` to `"???"`?

   

### 4.2 Building and modifying C programs

In


1.  Download the `lab-01-code.zip` file from the "Labs" section of the
    CITS3007 [Resources page](https://cits3007.arranstewart.io/resources/#labs).

    The best way to do so is from *within* your CITS3007 development environment
    (i.e. from within the virtual machine).

    You can do so by running

    ```bash
    $ wget https://cits3007.arranstewart.io/labs/lab-01-code.zip
    ```

    <div style="border: solid 2pt blue; background-color: hsla(241, 100%,50%, 0.1); padding: 1em; border-radius: 5pt; margin-top: 1em; margin-bottom: 1em">
    
    ::: block-caption
    
    Navigating the command-line and building C programs
    
    :::

    In this unit, we assume you understand how to navigate a Unix-like operating system using the command-line,
    how to perform basic file operations, and how to run commands like `wget` and `unzip`.

    If not, visit the [resources page][cits3007-resources] on the CITS3007 website, where
    you'll find recommendations of guides and tutorials on using the Linux command-line (e.g. William
    Shotts' online guide [*The Linux Command Line*][lincomm]).

    Ideally, you should also have completed CITS2002 Systems Programming, and be
    familiar with how to build C projects using [`gcc`][gcc] and [GNU Make][make]. If
    not, you should consult your C textbook and the [CITS2002
    materials][cits2002-make] (look for the material on "Developing C programs in multiple files")
    for basic usage guides for those tools.

    </div>

[gcc]: https://gcc.gnu.org
[make]: https://www.gnu.org/software/make/
[cits3007-resources]: https://cits3007.arranstewart.io/resources/ 
[cits2002-make]: https://teaching.csse.uwa.edu.au/units/CITS2002/schedule.php
[lincomm]: http://linuxcommand.org/tlcl.php


2.  Unzip the zip file into a directory of your choosing, using the `unzip`
    command. (See `man unzip` if you need details of how to use the `unzip` command.)

3.  Build the `test_leap` program by typing `make`, then try running it
    with various different command-line arguments:

    ```
    $ ./test_leap 1901
    $ ./test_leap 1900
    $ ./test_leap 2000
    $ ./test_leap -1
    $ ./test_leap -0
    $ ./test_leap 9223372036854775807
    $ ./test_leap 9223372036854775808
    $ ./test_leap foo
    $ ./test_leap foo bar
    ```

    What results do you get? Take a look at the code in `test_leap.c`,
    read the documentation for the `strtol` function (by running
    `man strtol`),
    and explain what is happening in each case.

   


5.  The code in `test_leap.c` contains an error. The correct algorithm
    for determining whether a year is a leap year is outlined at
    <https://en.wikipedia.org/wiki/Leap_year#Algorithm>, but the code in
    `test_leap.c` incorrectly reports that (for instance) 1900 is a leap
    year, when it is not. (Can you spot any other errors? Feel free to
    post in the Help3007 forum if you can.)

    Fix the code in `test_leap.c` and test your changes by trying the
    values from step 3 again.

    Compare your changes with another student's – are there any
    differences between how you fixed the program?

    


## 5. Moodle signup and/or login

For upcoming quizzes and tests, we'll be using the CSSE Moodle server,
available at <https://quiz.jinhong.org>.

There is currently one Moodle quiz available -- it is not assessed, but you should attempt
it, as the material covered is examinable later.  You can use that quiz to revise or refresh
your knowledge of C.

Visit <https://quiz.jinhong.org> and sign up with your UWA email address, then attempt the
quiz in your own time, if you don't finish it in the lab.

<div style="border: solid 2pt orange; background-color: hsl(22.35, 100%, 85%, 1); padding: 1em;">

::: block-caption

Moodle assessments

:::

Make sure you have signed up for Moodle, and added yourself to the CITS3007 section of Moodle,
well before the first assessment (in Week 4).

Late in week 3, teaching staff will remove students from the CITS3007 section of Moodle if
they are not currently enrolled in CITS3007 (as only currently enrolled students are
permitted to take the assessments), and the self-registration will be locked.

</div>


<!-- vim: syntax=markdown tw=92 smartindent :
-->
