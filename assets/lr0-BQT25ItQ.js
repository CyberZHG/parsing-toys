import"./__vite-browser-external-C7_NouPi.js";import{s as w}from"./choose_examples-BjAtfSqC.js";import{C as S,g as y,s as T,a as L,b,c as g,d as v}from"./location_hash-MzB8CwVA.js";function F(e){let i=!1,s=0,a=0,t={x:0,y:0,width:0,height:0};function r(){const n=e.getAttribute("viewBox");if(n){const o=n.split(/\s+/).map(Number);t={x:o[0],y:o[1],width:o[2],height:o[3]}}}function u(){e.setAttribute("viewBox",`${t.x} ${t.y} ${t.width} ${t.height}`)}e.addEventListener("mousedown",n=>{n.button===0&&(i=!0,s=n.clientX,a=n.clientY,r(),n.preventDefault())}),e.addEventListener("mousemove",n=>{if(!i)return;const o=n.clientX-s,c=n.clientY-a,l=t.width/e.clientWidth;t.x-=o*l,t.y-=c*l,u(),s=n.clientX,a=n.clientY}),e.addEventListener("mouseup",()=>{i=!1}),e.addEventListener("mouseleave",()=>{i=!1}),e.addEventListener("wheel",n=>{n.preventDefault(),r();const o=e.getBoundingClientRect(),c=n.clientX-o.left,l=n.clientY-o.top,x=t.x+c/e.clientWidth*t.width,E=t.y+l/e.clientHeight*t.height,h=n.deltaY>0?1.1:.9,p=t.width*h,f=t.height*h;t.x=x-c/e.clientWidth*p,t.y=E-l/e.clientHeight*f,t.width=p,t.height=f,u()},{passive:!1})}const C={example1:"Example 1: S -> S S + | S S * | a",example2:"Example 2: E -> E + T | T  T -> T * F | F  F -> ( E ) | id"},B={example1:`S -> S S + 
   | S S * 
   | a`,example2:`E -> E + T | T
T -> T * F | F
F -> ( E ) | id`},m=document.querySelector("#button-cfg-lr0"),d=document.querySelector("#automaton-svg");m.addEventListener("click",()=>{const e=document.querySelector("#cfg-lr0-error-message"),i=new S,s=y();if(i.parse(s))if(i.terminals().includes("¥"))e.textContent="The ¥ symbol is used as the end-of-input marker; please do not use it in the grammar.",e.parentElement.hidden=!1;else{if(i.nonTerminals().length>0){const a=i.computeLR0Automaton(),r=new DOMParser().parseFromString(a.toSVG(),"image/svg+xml");d.innerHTML=r.documentElement.innerHTML,d.setAttribute("viewBox",r.documentElement.getAttribute("viewBox"))}e.parentElement.hidden=!0}else e.textContent=i.errorMessage(),e.parentElement.hidden=!1;T(s)});function H(e){g(B[e]),m.click()}document.querySelector("#cfg-editor-intro").innerHTML=`
  Context free grammar input:
  <div class="text-gray-500 font-light text-sm">
    <ul>
      <li>The first symbol encountered in the grammar definition is treated as the start symbol</li>
      <li>The <code>¥</code> symbol is used as the end-of-input marker; please do not use it in the grammar.</li>
    </ul>
  </div>
`;w(C,H);L();b();F(d);g(v());m.click();
