// Menu lateral no celular
const toggle = document.getElementById("menuToggle");
const sidebar = document.getElementById("sidebar");
const overlay = document.getElementById("overlay");

function setMenu(open) {
  sidebar.classList.toggle("is-open", open);
  overlay.classList.toggle("is-open", open);
  toggle.setAttribute("aria-expanded", open);
  toggle.setAttribute("aria-label", open ? "Fechar menu" : "Abrir menu");
}

toggle.addEventListener("click", () => setMenu(!sidebar.classList.contains("is-open")));
overlay.addEventListener("click", () => setMenu(false));
document.addEventListener("keydown", (e) => { if (e.key === "Escape") setMenu(false); });
sidebar.querySelectorAll("a").forEach((a) => a.addEventListener("click", () => setMenu(false)));

// Ano atual no rodapé (só existe nas páginas que têm rodapé)
const year = document.getElementById("year");
if (year) year.textContent = new Date().getFullYear();

// ---------- Dados vindos do dados.json (gerado pelo programa em C) ----------
function el(tag, className, text) {
  const e = document.createElement(tag);
  if (className) e.className = className;
  if (text) e.textContent = text;
  return e;
}

function montarServicos(lista) {
  const box = document.getElementById("listaServicos");
  if (!box) return;
  box.replaceChildren(...lista.map((s) => {
    const a = el("a", "card");
    a.href = "agendamento.html";
    const ico = el("span", "card__icon", s.icone);
    ico.setAttribute("aria-hidden", "true");
    a.append(ico, el("h3", "", s.titulo), el("p", "", s.descricao));
    return a;
  }));
}

function montarDepoimentos(lista) {
  const box = document.getElementById("listaDepoimentos");
  if (!box) return;
  box.replaceChildren(...lista.map((d) => {
    const fig = el("figure", "testimonial");
    const nome = el("span");
    nome.append(el("strong", "", d.nome), el("small", "", d.pet));
    const cap = el("figcaption");
    cap.append(el("span", "avatar", d.iniciais), nome);
    fig.append(el("blockquote", "", d.texto), cap);
    return fig;
  }));
}

// Se o JSON não carregar (ex.: abrir o arquivo direto, sem servidor),
// o conteúdo que já está escrito no HTML continua aparecendo.
fetch("data/dados.json")
  .then((r) => { if (!r.ok) throw new Error(r.status); return r.json(); })
  .then((dados) => {
    montarServicos(dados.servicos);
    montarDepoimentos(dados.depoimentos);
  })
  .catch(() => console.info("data/dados.json não carregado; usando o conteúdo do HTML."));
