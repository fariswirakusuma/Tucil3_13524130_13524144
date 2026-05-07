## Build dan Run
- ### Kompilasi
Gunakan make untuk mengompilasi program:

```Bash
make build # untuk build
make run # untuk run
make build && make run # untuk build dan run
```

Jika berhasil, file executable akan muncul di direktori bin/.

Menjalankan Program
Jalankan program dengan argumen file peta:

```Bash
./bin/IceSlidingSolver maps/solvable_7x7.txt
```
- ### Kontrol GUI
  - Pencarian: Klik tombol algoritma (A*/UCS) untuk memulai searching.

  - Playback: Tekan [P] setelah status SUCCESS.

  - [Panah Kanan/Kiri]: Maju/Mundur satu langkah.

  - [Space]: Play/Pause otomatis.

  - [+/-]: Menambah/Mengurangi kecepatan playback.

  - [ESC]: Jump ke langkah spesifik.

- ### Navigasi:

  - [S]: Simpan solusi ke file .txt.

  - [B]: Kembali ke menu/pilihan peta.

  - [Q]: Keluar dari program.