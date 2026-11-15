// auth.js - controle de acesso do site. Carregue no <head>, antes do resto da página.
// Páginas públicas: index.html (login) e cadastro.html. As demais exigem login.
// Os dados ficam no localStorage: "usuarios", "animais", "agendamentos" e "usuarioLogado".
(() => {
  const lerLista = (chave) => {
    try { return JSON.parse(localStorage.getItem(chave)) || []; } catch { return []; }
  };
  const salvarLista = (chave, lista) => localStorage.setItem(chave, JSON.stringify(lista));
  const usuario = () => {
    const login = localStorage.getItem("usuarioLogado") || "";
    return lerLista("usuarios").find((u) => u.login === login) || null;
  };

  const pagina = location.pathname.split("/").pop() || "index.html";
  const ehLogin = pagina === "index.html";
  const publica = ehLogin || pagina === "cadastro.html";
  const user = usuario();

  window.Auth = { lerLista, salvarLista, usuario };

  if (!user && !publica) { location.replace("index.html"); return; }   // sem login: volta ao login
  if (user && ehLogin) { location.replace("inicio.html"); return; }    // já logado: vai para o início

  document.addEventListener("DOMContentLoaded", () => {
    const botao = document.querySelector(".sidebar__login");
    if (user && botao) {
      botao.textContent = "🚪 Sair";
      botao.title = `Conectado como ${user.nome}`;
      botao.href = "index.html";
      botao.addEventListener("click", () => localStorage.removeItem("usuarioLogado"));
    }
    if (!user) {   // visitante: só vê login e cadastro, sem menu de navegação
      const nav = document.querySelector(".sidebar .nav");
      if (nav) nav.hidden = true;
      const ag = document.querySelector(".mobilebar .btn");
      if (ag) ag.hidden = true;
    }
  });
})();
