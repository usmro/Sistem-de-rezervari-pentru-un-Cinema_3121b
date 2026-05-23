const API = 'http://localhost:8080/api';

const state = {
    user: JSON.parse(localStorage.getItem('cinema_user') || 'null'),
    loginTip: 'client'
};

function initNav() {
    const guest = document.getElementById('nav-guest');
    const userNav = document.getElementById('nav-user');
    const userLabel = document.getElementById('nav-username');
    if (!guest || !userNav) return;
    if (state.user) {
        guest.style.display = 'none';
        userNav.style.display = 'flex';
        if (userLabel) userLabel.textContent = state.user.username;
    } else {
        guest.style.display = 'flex';
        userNav.style.display = 'none';
    }
}

function showScreen(id) {
    document.querySelectorAll('.screen').forEach(s => s.classList.remove('active'));

    if (id === 'screen-login') {
        document.getElementById('login-username').value = '';
        document.getElementById('login-parola').value = '';
        document.getElementById('login-error').classList.add('hidden');
    } else if (id === 'screen-register') {
        document.getElementById('reg-username').value = '';
        document.getElementById('reg-parola').value = '';
        document.getElementById('reg-parola2').value = '';
        document.getElementById('reg-error').classList.add('hidden');
        document.getElementById('reg-success').classList.add('hidden');
    }

    const target = document.getElementById(id);
    if (target) {
        target.classList.add('active');
        if (id === 'screen-program') loadProgram();
        if (id === 'screen-account') loadAccount();
    }
}

function logout() {
    localStorage.removeItem('cinema_user');
    state.user = null;
    initNav();
    showScreen('screen-welcome');
}

function selectTab(el, tip) {
    document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
    el.classList.add('active');
    state.loginTip = tip;
}

async function handleLogin(e) {
    e.preventDefault();
    const username = document.getElementById('login-username').value.trim();
    const parola = document.getElementById('login-parola').value;
    const errEl = document.getElementById('login-error');

    errEl.classList.add('hidden');

    try {
        const res = await fetch(`${API}/login`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                username,
                parola,
                tip: state.loginTip
            })
        });

        const data = await res.json();

        if (!res.ok || data.status === 'eroare') {
            errEl.textContent = data.mesaj || 'Autentificare esuata.';
            errEl.classList.remove('hidden');
            return;
        }

        state.user = data.user;
        localStorage.setItem('cinema_user', JSON.stringify(data.user));
        initNav();

        if (state.loginTip === 'admin' || state.user.rol === 'Admin') {
            window.location.href = 'admin.html';
        } else {
            showScreen('screen-account');
        }
    } catch (err) {
        errEl.textContent = 'Serverul nu raspunde. Porneste CinemaServer.';
        errEl.classList.remove('hidden');
    }
}

async function handleRegister(e) {
    e.preventDefault();
    const username = document.getElementById('reg-username').value.trim();
    const parola = document.getElementById('reg-parola').value;
    const parola2 = document.getElementById('reg-parola2').value;
    const errEl = document.getElementById('reg-error');
    const okEl = document.getElementById('reg-success');

    errEl.classList.add('hidden');
    okEl.classList.add('hidden');

    if (parola !== parola2) {
        errEl.textContent = 'Parolele nu coincid.';
        errEl.classList.remove('hidden');
        return;
    }
    if (username.length < 3) {
        errEl.textContent = 'Username-ul trebuie sa aiba minim 3 caractere.';
        errEl.classList.remove('hidden');
        return;
    }

    try {
        const res = await fetch(`${API}/register`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ username, parola })
        });

        const data = await res.json();

        if (!res.ok || data.status === 'eroare') {
            errEl.textContent = data.mesaj || 'Eroare la inregistrare.';
            errEl.classList.remove('hidden');
            return;
        }

        okEl.textContent = 'Cont creat! Te poti autentifica acum.';
        okEl.classList.remove('hidden');
        document.getElementById('reg-username').value = '';
        document.getElementById('reg-parola').value = '';
        document.getElementById('reg-parola2').value = '';

        setTimeout(() => showScreen('screen-login'), 1500);

    } catch (err) {
        errEl.textContent = 'Serverul nu raspunde.';
        errEl.classList.remove('hidden');
    }
}

let accReservations = [];

async function loadAccount() {
    if (!state.user) { showScreen('screen-login'); return; }

    const titleEl = document.getElementById('acc-title');
    const subEl = document.getElementById('acc-sub');
    const badgeEl = document.getElementById('acc-card-badge');
    const listEl = document.getElementById('acc-rezervari');

    if (titleEl) titleEl.textContent = state.user.username;
    if (subEl) subEl.textContent = 'Rezervările tale';

    // Show / hide fidelity badge
    if (badgeEl) {
        const hasCard = state.user.cardFidelitate || state.user.areCardFidelitate ||
            state.user.card || false;
        badgeEl.style.display = hasCard ? 'block' : 'none';
    }

    if (!listEl) return;
    listEl.innerHTML = '<p style="color:#888;font-size:13px">Se încarcă...</p>';

    try {
        const res = await fetch(`${API}/rezervarile-mele?username=${encodeURIComponent(state.user.username)}`);

        if (!res.ok) {
            // Endpoint lipsă (server vechi) sau eroare de server
            console.error('rezervarile-mele HTTP', res.status);
            listEl.innerHTML = '<p style="color:#f87171;font-size:13px;margin-top:8px">Nu pot încărca rezervările (server vechi sau oprit). Recompilează și repornește CinemaServer.</p>';
            return;
        }

        const data = await res.json();

        if (!Array.isArray(data)) {
            console.error('rezervarile-mele: răspuns neașteptat', data);
            listEl.innerHTML = '<p style="color:#f87171;font-size:13px;margin-top:8px">Răspuns invalid de la server.</p>';
            return;
        }

        accReservations = data;
        const filters = document.getElementById('acc-filters');

        if (!data.length) {
            if (filters) filters.style.display = 'none';
            listEl.innerHTML = '<p style="color:#888;font-size:13px;margin-top:8px">Nu ai nicio rezervare momentan.</p>';
            return;
        }

        if (filters) filters.style.display = 'flex';
        renderAccount();

    } catch (err) {
        listEl.innerHTML = '<p style="color:#f87171;font-size:13px">Eroare la încărcarea rezervărilor.</p>';
    }
}

// Parse "YYYY-MM-DD HH:MM" -> Date (local)
function parseDataOraJS(s) {
    if (!s) return null;
    const m = String(s).match(/(\d{4})-(\d{2})-(\d{2})[ T](\d{2}):(\d{2})/);
    if (!m) return null;
    return new Date(+m[1], +m[2] - 1, +m[3], +m[4], +m[5]);
}

function renderAccount() {
    const listEl = document.getElementById('acc-rezervari');
    if (!listEl) return;

    const search = (document.getElementById('acc-search')?.value || '').toLowerCase().trim();
    const status = document.getElementById('acc-status')?.value || 'toate';
    const period = document.getElementById('acc-period')?.value || 'toate';
    const sort = document.getElementById('acc-sort')?.value || 'recent';
    const now = new Date();

    let data = accReservations.filter(r => {
        const anulata = r.anulata || r.status === 'anulata';
        if (status === 'active' && anulata) return false;
        if (status === 'anulate' && !anulata) return false;
        const d = parseDataOraJS(r.dataOra || r.data);
        if (period === 'viitoare' && d && d < now) return false;
        if (period === 'trecute' && d && d >= now) return false;
        if (search && !(r.titluFilm || r.film || '').toLowerCase().includes(search)) return false;
        return true;
    });

    data.sort((a, b) => {
        if (sort === 'film') return (a.titluFilm || '').localeCompare(b.titluFilm || '');
        const da = parseDataOraJS(a.dataOra || a.data)?.getTime() || 0;
        const db = parseDataOraJS(b.dataOra || b.data)?.getTime() || 0;
        return sort === 'vechi' ? da - db : db - da;
    });

    // Mic rezumat (nr. bilete active + sumă cheltuită)
    const active = accReservations.filter(r => !(r.anulata || r.status === 'anulata'));
    const totalCheltuit = active.reduce((a, r) => a + (r.pretFinal || 0), 0);
    const sumEl = document.getElementById('acc-summary');
    if (sumEl) sumEl.textContent = `${active.length} bilete active · ${totalCheltuit.toFixed(2)} RON cheltuiți`;

    if (!data.length) {
        listEl.innerHTML = '<p style="color:#888;font-size:13px;margin-top:8px">Nicio rezervare pentru filtrele selectate.</p>';
        return;
    }

    listEl.innerHTML = data.map(r => {
        const anulata = r.anulata || r.status === 'anulata';
        const d = parseDataOraJS(r.dataOra || r.data);
        const trecuta = d && d < now && !anulata;
        const canCancel = !anulata && !trecuta;

        const statusBadge = anulata
            ? `<span style="display:inline-block;padding:2px 8px;background:rgba(239,68,68,.15);border:1px solid rgba(239,68,68,.4);border-radius:10px;font-size:11px;color:#f87171">Anulată</span>`
            : trecuta
                ? `<span style="display:inline-block;padding:2px 8px;background:rgba(148,163,184,.15);border:1px solid rgba(148,163,184,.35);border-radius:10px;font-size:11px;color:#94a3b8">Trecută</span>`
                : `<span style="display:inline-block;padding:2px 8px;background:rgba(34,197,94,.12);border:1px solid rgba(34,197,94,.3);border-radius:10px;font-size:11px;color:#22c55e">Viitoare</span>`;

        const pretBadge = r.pretFinal != null
            ? `<span style="font-size:12px;color:#aaa">· ${(r.pretFinal).toFixed(2)} RON</span>` : '';

        const cancelBtn = canCancel
            ? `<button onclick="cancelRezervare('${escHtml(r.id)}')"
                    style="padding:6px 14px;background:transparent;border:1px solid rgba(239,68,68,.45);color:#f87171;border-radius:6px;cursor:pointer;font-family:inherit;font-size:12px;white-space:nowrap">
                    Anulează
               </button>`
            : '';

        const dim = (anulata || trecuta) ? 'opacity:.6' : '';

        return `
        <div style="display:flex;justify-content:space-between;align-items:center;gap:12px;
                    padding:14px 16px;background:rgba(255,255,255,.04);border:1px solid rgba(255,255,255,.09);
                    border-radius:8px;margin-bottom:10px;${dim}">
            <div style="min-width:0">
                <div style="font-weight:600;font-size:14px;margin-bottom:4px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis">
                    🎬 ${escHtml(r.titluFilm || r.film || '—')}
                </div>
                <div style="font-size:12px;color:#aaa;line-height:1.6">
                    📅 ${escHtml(r.dataOra || r.data || '—')}&nbsp;&nbsp;
                    🏛️ ${escHtml(r.salaNume || r.sala || '—')}&nbsp;&nbsp;
                    💺 Rând ${escHtml(String(r.rand || '—'))}, Loc ${escHtml(String(r.loc || '—'))}
                </div>
                <div style="margin-top:5px;display:flex;align-items:center;gap:8px">
                    ${statusBadge}
                    ${pretBadge}
                    <span style="font-size:12px;color:#aaa">ID: <code style="font-size:11px;color:#888">${escHtml(r.id || '—')}</code></span>
                </div>
            </div>
            <div style="flex-shrink:0">${cancelBtn}</div>
        </div>`;
    }).join('');
}

async function cancelRezervare(idRez) {
    if (!confirm('Ești sigur că vrei să anulezi această rezervare?')) return;

    try {
        const res = await fetch(`${API}/anuleaza`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ id: idRez, username: state.user?.username })
        });
        const data = await res.json();

        if (data.status === 'ok' || res.ok) {
            loadAccount(); // refresh list
        } else {
            alert(data.mesaj || 'Eroare la anulare.');
        }
    } catch (err) {
        alert('Serverul nu răspunde.');
    }
}

let programData = [];

async function loadProgram() {
    const container = document.getElementById('program-list');
    const filters = document.getElementById('prog-filters');
    container.innerHTML = '<p class="loading">Se incarca...</p>';
    if (filters) filters.style.display = 'none';

    try {
        const res = await fetch(`${API}/proiectii/viitoare`);
        const data = await res.json();

        if (!Array.isArray(data) || !data.length) {
            container.innerHTML =
                '<p class="loading">Nu exista proiectii programate.</p>';
            return;
        }

        programData = data;

        // Populează filtrele (gen + zi) o singură dată din date
        const genuri = new Set();
        const zile = new Set();
        data.forEach(p => {
            (p.film.gen || '').split('/').forEach(g => { g = g.trim(); if (g) genuri.add(g); });
            const zi = (p.dataOra || '').slice(0, 10);
            if (zi) zile.add(zi);
        });
        const genSel = document.getElementById('prog-gen');
        if (genSel) genSel.innerHTML = '<option value="">Toate genurile</option>' +
            [...genuri].sort().map(g => `<option value="${escHtml(g)}">${escHtml(g)}</option>`).join('');
        const ziSel = document.getElementById('prog-zi');
        if (ziSel) ziSel.innerHTML = '<option value="">Toate zilele</option>' +
            [...zile].sort().map(z => `<option value="${escHtml(z)}">${escHtml(z)}</option>`).join('');

        if (filters) filters.style.display = 'flex';
        renderProgram();

    } catch (err) {
        container.innerHTML =
            '<p class="loading">Eroare la incarcarea programului.</p>';
    }
}

function renderProgram() {
    const container = document.getElementById('program-list');
    if (!container) return;

    const search = (document.getElementById('prog-search')?.value || '').toLowerCase().trim();
    const gen = document.getElementById('prog-gen')?.value || '';
    const format = document.getElementById('prog-format')?.value || '';
    const zi = document.getElementById('prog-zi')?.value || '';

    let data = programData.filter(p => {
        if (search && !(p.film.titlu || '').toLowerCase().includes(search)) return false;
        if (gen && !((p.film.gen || '').includes(gen))) return false;
        if (format && p.film.tip !== format) return false;
        if (zi && !(p.dataOra || '').startsWith(zi)) return false;
        return true;
    });

    const cntEl = document.getElementById('prog-count');
    if (cntEl) cntEl.textContent = `${data.length} proiecții`;

    if (!data.length) {
        container.innerHTML = '<p class="loading">Nicio proiecție pentru filtrele selectate.</p>';
        return;
    }

    container.innerHTML = data.map(p => {
        const tipBadge = p.film.tip === '3D'
            ? '<span class="badge badge-3d">3D</span>' : '';
        const salaBadge = p.sala.tip === 'VIP'
            ? '<span class="badge badge-vip">VIP</span>'
            : p.sala.tip === 'IMAX'
                ? '<span class="badge badge-imax">IMAX</span>'
                : '';

        return `
        <div class="proiectie-card">
            <div class="film-titlu">
                ${escHtml(p.film.titlu)} ${tipBadge}
            </div>
            <div class="meta">
                <div>📅 ${escHtml(p.dataOra)}</div>
                <div>🎭 ${escHtml(p.film.gen)}
                     &nbsp;·&nbsp; ${p.film.durata} min</div>
                <div>🏛️ ${escHtml(p.sala.nume)} ${salaBadge}</div>
                <div>🔊 ${escHtml(p.formatAudio)}</div>
            </div>
            <div class="pret">
                de la ${p.film.pretBaza.toFixed(2)} RON
            </div>
        </div>`;
    }).join('');
}

function escHtml(str) {
    return String(str)
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;');
}

// ── Boot ─────────────────────────────────────────────────────────────────────
document.addEventListener('DOMContentLoaded', () => {
    initNav();
});