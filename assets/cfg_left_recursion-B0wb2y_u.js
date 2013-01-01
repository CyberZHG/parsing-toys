import"./__vite-browser-external-BgPlA5PU.js";import{s as o}from"./choose_examples-BjAtfSqC.js";import{g as s,s as m,a as n}from"./cfg_editor-Bof7UeYh.js";import{u as l,s as c}from"./cfg_result-CcZunNg_.js";import{C as p,s as i,a as u,g as f}from"./location_hash-DCeX6uqc.js";const x={example1:"Example 1: S -> S S + | S S * | a",example2:"Example 2: S -> ( L ) | a  L -> L , S | S",example3:"Example 3: A -> C | c d  B -> C | c d  C -> A | B | c d",example4:"Example 4: bexpr -> bexpr or bterm | bterm  bterm -> bterm and bfactor | bfactor  bfactor -> not bfactor | ( bexpr ) | true | false",example5:"Example 5: A -> B  B -> C  C -> D  D -> A"},b={example1:`S -> S S + 
   | S S * 
   | a`,example2:`S -> ( L ) 
   | a L -> L , S 
   | S`,example3:`A -> C | c d
B -> C | c d
C -> A | B | c d`,example4:`  bexpr -> bexpr or bterm
         | bterm
  bterm -> bterm and bfactor
         | bfactor
bfactor -> not bfactor
         | ( bexpr )
         | true
         | false`,example5:`A -> B
B -> C
C -> D
D -> A`},a=document.querySelector("#button-cfg-left-recursion");a.addEventListener("click",()=>{const e=document.querySelector("#cfg-left-recursion-error-message"),t=new p,r=s();t.parse(r)?(t.leftRecursionElimination()?e.parentElement.hidden=!0:(e.textContent=t.errorMessage(),e.parentElement.hidden=!1),l(t.toString(),t.terminals(),t.nonTerminals())):(e.textContent=t.errorMessage(),e.parentElement.hidden=!1),i(r)});function d(e){n(b[e]),a.click()}o(x,d);m();c();u();n(f());a.click();
