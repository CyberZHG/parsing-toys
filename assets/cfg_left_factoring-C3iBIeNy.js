import"./__vite-browser-external-B-rQEfPv.js";import{C as u,g as m,u as p,s as d,a as E,b as g,c as x,d as c,e as f}from"./location_hash-DzrqQBRv.js";function C(e,t){const a=document.querySelector("#examples-menu"),i=document.querySelector("#examples-menu-button-show");let o="";Object.keys(e).forEach(n=>{o+=`<li class="px-3 py-2 hover:bg-gray-100 cursor-pointer" data-value="${n}">${e[n]}</li>`}),a.innerHTML=o,a.addEventListener("click",n=>{const l=n.target;if(!l)return;const r=l.closest("li");if(!r)return;const S=r.dataset.value;a.classList.toggle("hidden"),t(S)}),i.addEventListener("click",()=>{a.classList.toggle("hidden")})}const h={example1:"Example 1: S -> i E t S | i E t S e S | a  E -> b",example2:"Example 2: S -> S S + | S S * | a",example3:"Example 3: S -> S + S | S S | ( S ) | S * | a",example4:"Example 4: A -> id | B | a  B -> C  C -> D  D -> id b"},L={example1:`S -> i E t S 
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
D -> id b`},s=document.querySelector("#button-cfg-left-factoring");s.addEventListener("click",()=>{const e=document.querySelector("#cfg-left-factoring-error-message"),t=new u,a=m();t.parse(a)?(t.leftFactoring(),p(t.toString(),t.terminals(),t.nonTerminals()),e.parentElement.hidden=!0):(e.textContent=t.errorMessage(),e.parentElement.hidden=!1),d(a)});function b(e){c(L[e]),s.click()}C(h,b);E();g();x();c(f());s.click();
