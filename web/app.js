const API = 'http://localhost:8080/api';

const state = {
    user: null,
    loginTip: 'client'
};

function showScreen(id) {
    document.querySelectorAll('.screen').forEach(s => {
        s.classList.remove('active');
    });

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
    }
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

        if (state.loginTip === 'admin' || state.user.rol === 'Admin') {
            window.location.href = 'admin.html';
        } else {
            window.location.href = 'client.html';
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

async function loadProgram() {
    const container = document.getElementById('program-list');
    container.innerHTML = '<p class="loading">Se incarca...</p>';

    try {
        const res = await fetch(`${API}/proiectii/viitoare`);
        const data = await res.json();

        if (!data.length) {
            container.innerHTML =
                '<p class="loading">Nu exista proiectii programate.</p>';
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

    } catch (err) {
        container.innerHTML =
            '<p class="loading">Eroare la incarcarea programului.</p>';
    }
}

function escHtml(str) {
    return String(str)
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;');
}