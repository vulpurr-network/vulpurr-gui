<p align="center"><img src="https://vulpurr.com/logo.svg" width="96" alt="Vulpurr"></p>

<h1 align="center">Vulpurr Wallet</h1>

<p align="center">Desktop wallet for Vulpurr (VUP) with a built-in node, one-click P2Pool mining and signed auto-updates.<br>
<a href="https://vulpurr.com/download">Download for Windows</a> · <a href="https://vulpurr.com">Website</a> · <a href="https://t.me/vulpurr">Telegram</a></p>

**English** · [Русский](#русский)

A fork of the [Monero GUI](https://github.com/monero-project/monero-gui); the upstream history is kept and all Vulpurr changes sit on top. The original README is in [README.monero.md](README.monero.md). The node and wallet library come from the [`monero`](monero) submodule, which points to [vulpurr-network/vulpurr](https://github.com/vulpurr-network/vulpurr).

> ⚠️ The network is in test mode: balances will be reset before the official launch.

## What is different from the Monero GUI

- Vulpurr branding, the `vulpurr:` URI scheme, settings and wallets kept separately from Monero (`/Vulpurr/wallets`).
- **P2Pool built in**: Advanced → Mining → P2Pool starts mining with 0% fee, rewards arrive directly in the block. The Mini/Main/Nano chooser is gone: the wallet always uses the main Vulpurr sidechain.
- **Signed auto-updates** ([`src/main/VulpurrUpdater.cpp`](src/main/VulpurrUpdater.cpp)): the wallet checks for updates at start and every 30 minutes, verifies the manifest signature (CryptoNote Ed25519 key) and the SHA-256 of every file, and installs only if both match. Several mirrors are tried in turn.
- Simple mode connects to `node.vulpurr.com:27089`.
- The fiat price display is disabled: there is no market price for VUP yet, and showing Monero's would be misleading.
- Rebranding helpers live in [`branding/`](branding).

## Building

Windows binaries are built with the Monero GUI Docker toolchain:

```bash
git clone --recursive https://github.com/vulpurr-network/vulpurr-gui.git
cd vulpurr-gui
docker build -t monero:build-env-windows --build-arg THREADS=$(nproc) --file Dockerfile.windows .
docker run --rm -it -v $PWD:/monero-gui -w /monero-gui monero:build-env-windows sh -c 'make depends root=/depends target=x86_64-w64-mingw32 tag=win-x64 -j$(nproc)'
```

The result is `build/x86_64-w64-mingw32/release/bin/monero-wallet-gui.exe` (shipped as `vulpurr-wallet.exe`) plus `vulpurrd.exe`. Linux and macOS builds are coming.

## License

BSD 3-Clause, like the Monero GUI ([LICENSE](LICENSE)). Thanks to the Monero GUI developers.

---

## Русский

Кошелёк Vulpurr для компьютера: встроенная нода, майнинг через P2Pool в один клик и автообновления с проверкой подписи. Форк [Monero GUI](https://github.com/monero-project/monero-gui): история сохранена, изменения Vulpurr лежат поверх.

- **P2Pool внутри:** Дополнительно → Майнинг → P2Pool, комиссии нет, награда приходит прямо в блоке.
- **Автообновления с подписью:** кошелёк проверяет обновления при запуске и каждые 30 минут, сверяет подпись манифеста и SHA-256 каждого файла и ставит обновление, только если всё сошлось.
- В простом режиме подключается к `node.vulpurr.com:27089`. Курс в долларах выключен: настоящей цены у VUP пока нет.

Сборка под Windows — через Docker, как у Monero GUI (команды выше). Сборки под Linux и macOS скоро. Лицензия BSD 3-Clause.
