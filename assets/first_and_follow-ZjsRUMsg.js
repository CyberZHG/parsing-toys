import"./__vite-browser-external-BgPlA5PU.js";import{s as p}from"./choose_examples-BjAtfSqC.js";import{g as u,s as f,a as i}from"./cfg_editor-Bof7UeYh.js";import{C as g,s as x,a as E,g as T}from"./location_hash-DCeX6uqc.js";const y={example1:"Example 1: E -> T E'  T -> F T'  E' -> + T E' | ε  T' -> * F T' | ε  F -> ( E ) | id",example2:"Example 2: S -> A B C  A -> a | ε  B -> b | ε  C -> c"},F={example1:` E -> T E'
 T -> F T'
E' -> + T E'
    | ε
T' -> * F T'
    | ε
 F -> ( E )
    | id`,example2:`S -> A B C
A -> a
   | ε
B -> b
   | ε
C -> c`},l=document.querySelector("#button-cfg-first-and-follow");l.addEventListener("click",()=>{const o=document.querySelector("#cfg-first-and-follow-result"),n=document.querySelector("#cfg-first-and-follow-error-message"),r=new g,c=u();if(r.parse(c)){const t=r.computeFirstAndFollowSet();let e="";for(let a=0;a<t.size();++a){const s=t.symbolAt(a),d=t.getFirstSet(s).join(" ").replace(/(^|\s)ε(?=\s|$)/g,'$1<span class="text-green-700">ε</span>'),m=t.getFollowSet(s).join(" ").replace(/(^|\s)\$(?=\s|$)/g,'$1<span class="text-green-700">$</span>');e+='<tr class="odd:bg-white even:bg-gray-50 border-none">',e+=`  <td class="px-4 py-2 text-center font-mono">${s}</td>`,t.getNullable(s)?e+='  <td class="px-4 py-2 text-center">✓</td>':e+='  <td class="px-4 py-2"></td>',e+=`  <td class="px-4 py-2 text-center font-mono">${d}</td>`,e+=`  <td class="px-4 py-2 text-center font-mono">${m}</td>`,e+="</tr>"}o.innerHTML=e,n.parentElement.hidden=!0}else n.textContent=r.errorMessage(),n.parentElement.hidden=!1;x(c)});function h(o){i(F[o]),l.click()}document.querySelector("#cfg-editor-intro").innerHTML=`
  Context free grammar input:
  <div class="text-gray-500 font-light text-sm">
    <ul>
      <li>The first symbol encountered in the grammar definition is treated as the start symbol</li>
      <li>The <code>¥</code> symbol is used as the end-of-input marker; please do not use it in the grammar.</li>
    </ul>
  </div>
`;p(y,h);f();E();i(T());l.click();
