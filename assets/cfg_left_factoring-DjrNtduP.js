import"./__vite-browser-external-CpYqa-Ba.js";import{C as u,g as m,u as p,s as d,a as E,b as g,c as x,d as i,e as f}from"./location_hash-CbENGIbH.js";function C(e,t){const a=document.querySelector("#examples-menu"),l=document.querySelector("#examples-menu-button-show");let r="";Object.keys(e).forEach(n=>{r+=`<li class="px-3 py-2 hover:bg-gray-100 cursor-pointer" data-value="${n}">${e[n]}</li>`}),a.innerHTML=r,a.addEventListener("click",n=>{const c=n.target;if(!c)return;const s=c.closest("li");if(!s)return;const S=s.dataset.value;a.classList.toggle("hidden"),l.innerHTML=s.textContent.trim()+"<span>▾</span>",t(S)}),l.addEventListener("click",()=>{a.classList.toggle("hidden")})}const L={example1:"Example 1: S -> i E t S | i E t S e S | a  E -> b",example2:"Example 2: S -> S S + | S S * | a",example3:"Example 3: S -> S + S | S S | ( S ) | S * | a",example4:"Example 4: A -> id | B | a  B -> C  C -> D  D -> id b"},h={example1:`S -> i E t S 
   | i E t S e S 
   | a
E -> b`,example2:`S -> S S + 
   | S S * 
   | a`,example3:`S -> S + S 
   | S S 
   | ( S ) 
   | S * 
   | a`,example4:`A -> id | B | a
B -> C
C -> D
D -> id b`},o=document.querySelector("#button-cfg-left-factoring");o.addEventListener("click",()=>{const e=document.querySelector("#cfg-left-factoring-error-message"),t=new u,a=m();t.parse(a)?(t.leftFactoring(),p(t.toString(),t.terminals(),t.nonTerminals()),e.parentElement.hidden=!0):(e.textContent=t.errorMessage(),e.parentElement.hidden=!1),d(a)});function M(e){i(h[e]),o.click()}C(L,M);E();g();x();i(f());o.click();
