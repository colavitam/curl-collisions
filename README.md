# Curl-P Collisions

This project provides an efficient implementation of Heilman et al.'s collision attack on IOTA's now-deprecated hash function, Curl-P. It can be used to generate novel collisions in the hash function in minutes to hours depending on hardware. Please see our [report](https://github.com/colavitam/curl-collisions/raw/master/paper.pdf) for a detailed coverage of the methodology behind its implementation.

## Usage
To generate collisions using this project, clone the repository locally and build with `make`. Start the program with `./collide -t NUM_THREADS` where `NUM_THREADS` is the suitable number of threads for your machine.

## Example collisions

```
hash(ACMUXEIFDOIVQMVZNXPNWGSA9JGCN9RIMWOYNFLAVLBKRJPKRAYFCGSD9CAJEFVPHIWRZEKQHUHCAKKSTXMDZMMVEVVCTQFRTMDR9QLPG9QUWBHBQBVOPDWDIOFUWBK9IREKOUVRHDODLLXCLMJWZZXENYXDUSVDGU)=
hash(ACMUXEIFDOIVQMVZNXPNWGSA9JGCN9RIMWOYNFLAVLBKRJPKRAYFCGSD9CAJEFVPHIWRZEKQHUHCAKKSTXMDZMMVEVVCTQFRTMDR9QLPG9QUWBHBQBVOPDWDIOGUWBK9IREKOUVRHDODLLXCLMJWZZXENYXDUSVDGU)=
BUEXRNXFUP9HUMBOJWJZBQKDTZKOUVUXSJAXGKMNH9I9EWNBXPBCFNEPBFCQFDYZZCBMXOTP9DOIMKEZ9

hash(ACMUXEIFDOIVQMVZNXPNWGSA9JGCN9RIMWOYNFLAVLBKRJPKRAYFCGSD9CAJEFVPHIWRZEKQHUHCAKKSTFYYHYMOGOVCTIYRVML9IKFSGCPSYBK9RDKNGBTVXIRIB9FBJOPLPUKRQMLVUOXDGKGDZ9FWWEXDXGJPYU)=
hash(ACMUXEIFDOIVQMVZNXPNWGSA9JGCN9RIMWOYNFLAVLBKRJPKRAYFCGSD9CAJEFVPHIWRZEKQHUHCAKKSTFYYHYMOGOVCTIYRVML9IKFSGCPSYBK9RDKNGBTVXISIB9FBJOPLPUKRQMLVUOXDGKGDZ9FWWEXDXGJPYU)=
XYXHZSVKLZFZQSLXCGSDZHYLPBNXVIDUMPTCWKUZSDPQMTETH9UFHPQAAWLNE9PTR9KBITPUU9ZOQXBTZ

hash(ACMUXEIFDOIVQMVZNXPNWGSA9JGCN9RIMWOYNFLAVLBKRJPKRAYFCGSD9CAJEFVPHIWRZEKQHUHCAKKSTLGGEAMPJQQDUZERLMKRXNXDS9OUDDKY9KKYNKTYU99IWWFXHPGLELTRRGOAIL9DMJH9Y9GOGEEMXSMDPR)=
hash(ACMUXEIFDOIVQMVZNXPNWGSA9JGCN9RIMWOYNFLAVLBKRJPKRAYFCGSD9CAJEFVPHIWRZEKQHUHCAKKSTLGGEAMPJQQDUZERLMKRXNXDS9OUDDKY9KKYNKTYU9AIWWFXHPGLELTRRGOAIL9DMJH9Y9GOGEEMXSMDPR)=
UT9GBOHGMYVVMXPDQWVDIZFDLFUGI9XNSL9LVTGKKLMBBJTBQ9PGDXGXLPHOQFPIMNIZAPHSIAVRDV9BX

hash(ACMUXEIFDOIVQMVZNXPNWGSA9JGCN9RIMWOYNFLAVLBKRJPKRAYFCGSD9CAJEFVPHIWRZEKQHUHCAKKSTISJNJGNFTPXNANSTMCRIKXMJXRNXAIBZUDONDBMXUXFBAJCMUPTGVV9QDRGIXCWJKIBDZFXYOYMUSDGPU)=
hash(ACMUXEIFDOIVQMVZNXPNWGSA9JGCN9RIMWOYNFLAVLBKRJPKRAYFCGSD9CAJEFVPHIWRZEKQHUHCAKKSTISJNJGNFTPXNANSTMCRIKXMJXRNXAIBZUDONDBMXUYFBAJCMUPTGVV9QDRGIXCWJKIBDZFXYOYMUSDGPU)=
DHSPNMAPHELGLFBRXBGXN9NLLLRVQRPZYN9NVKLOXSIJQ9NVGOQBIUPVRSBQRDCMBQMIBTIMIHKPWLHWU

hash(ACMUXEIFDOIVQMVZNXPNWGSA9JGCN9RIMWOYNFLAVLBKRJPKRAYFCGSD9CAJEFVPHIWRZEKQHUHCAKKSTFDVNYJTHVN9SQYZTVVICZCVACPQWDLYJCDEXLWDUUOCZ9MWLNEVETBQAGOARRXDIGIXYQEPGPEVIYMADU)=
hash(ACMUXEIFDOIVQMVZNXPNWGSA9JGCN9RIMWOYNFLAVLBKRJPKRAYFCGSD9CAJEFVPHIWRZEKQHUHCAKKSTFDVNYJTHVN9SQYZTVVICZCVACPQWDLYJCDEXLWDUUPCZ9MWLNEVETBQAGOARRXDIGIXYQEPGPEVIYMADU)=
GJXEJZISSTXJGLS99RVEFHOZJITZWCRDFVTDSAYGNBMMXSYXNRXPHRYPDIZWFILJDBV9QVHMIOAUSINNB

hash(ACMUXEIFDOIVQMVZNXPNWGSA9JGCN9RIMWOYNFLAVLBKRJPKRAYFCGSD9CAJEFVPHIWRZEKQHUHCAKKSTLSVNSGRLSUZNROAVKM9OZOGMCQSXBJY9KLNYVQYUUUUWAEYFNWCOKURQJFV9RXCKKLB9RXGXGNVOPAVGU)=
hash(ACMUXEIFDOIVQMVZNXPNWGSA9JGCN9RIMWOYNFLAVLBKRJPKRAYFCGSD9CAJEFVPHIWRZEKQHUHCAKKSTLSVNSGRLSUZNROAVKM9OZOGMCQSXBJY9KLNYVQYUUVUWAEYFNWCOKURQJFV9RXCKKLB9RXGXGNVOPAVGU)=
LRK9AHITJUEMPFCPVGFUZRNVWIF9TSYOJROYAZBENSWGSC9PDIHUAPKFTJECCPLOWJHSLW9SOTQQ9EKIP
```

## Example full-state collision prefixes

```
PJGZBOAWTZGMTXBKRFQJMBWNPIKNPMBNHFMMPDZGVB9XXJ9MCJAIQKXAHRMGCWN9XL9SIYZC9TUGFEBSK9GBNRYYAXXRWCPGTZR9XRIHXMYGRTEAHUSYGVKDSAUW9VTVJMMJXRLZRNZRPMEOFYAVBTHM9GZYEGOWQT
PJGZBOAWTZGMTXBKRFQJMBWNPIKNPMBNHFMMPDZGVB9XXJ9MCJAIQKXAHRMGCWN9XL9SIYZC9TUGFEBSK9GBNRYYAXXRECPGTZR9XRIHXMYGRTEAHUSYGVKDSAUW9VTVJMMJXRLZRNZRPMEOFYAVBTHM9GZYEGOWQT

ESXLBATKVGFXMBSFWMCWLLOOKIFPHWE9UKUJIYHEIKOPADEJZDEZDFGMQASRYPITOYBMJXHISGALBSVBWIEHUIZ9JPZVJAMRNCCHBBWNLOBWQLHCWUVDXS9RXGGWDCMBXNHCICDSAOQYOBQHEBQFJAWPWTAQNZMRLJ
ESXLBATKVGFXMBSFWMCWLLOOKIFPHWE9UKUJIYHEIKOPADEJZDEZDFGMQASRYPITOYBMJXHISGALBSVBWIEHUIH9JPZVJAMRNCCHBBWNLOBWQLHCWUVDXS9RXGGWDCMBXNHCICDSAOQYOBQHEBQFJAWPWTAQNZMRLJ

DGUNWWBDYJTMZTQCZA9YCUBMEQOAYGQQARNPARQQMBIECEVABJQMSCNKORWK9OIGLPFNTOUD9JJBVWCDBHCSRITVNWMHMTOTCJOPOPJGXWGGQBUKUPVD9OAQLNLMVYAMOXNZPZOIHW9HTWIGAAU9OKRISOPBSXLMKT
DGUNWWBDYJTMZTQCZA9YCUBMEQOAYGQQARNPARQQMBIECEVABJQMSCNKORWK9OIGLPFNTOUD9JJBVWCDBHCSRITVNWMHMTOTCJOPOPJGXWGGQBUKUPVD9OAQLNLMVYAMOXNZPZOLHW9HTWIGAAU9OKRISOPBSXLMKT

JXALEYWJKUKMIFXAEWTCHPPNCJWI9THDCOPIWEK9XYQUZEKHPTLUPQLUXRXWFALCRMSRLBMKQILBBFISIEASHUUYFN9TVFAOEJGNQUWPPCUAVNCBICQCXUQFUTSIYPAHRHRQIJQFJMRS9GJQGJKTSTETQJMLBZZGQO
JXALEYWJKUKMIFXAEWTCHPPNCJWI9THDCOPIWEK9XYQUZEKHPTLUPQLUXRXWFALCRMSRLBMKQILBBFISIEASHUUYFN9TVFAOEJGNQUWPPCUAVNCBICQCXUQFUTSIYPAHRHRQIJQFJMRS9GJQGJKTSTETQMMLBZZGQO

ZRQVFYSECZEIDZTMECCRLBCHBGHWRCGGREWAREWD9TLKZXOKPKAEZCTMNFMUXALVLFPAGQBSYDOAGNZJHGPFOQIAADNAQMAUOSXNH9RGUXUIJ9LR9NZAOB9XRJONKZZHWOZUMPZINXPQXTXHJMWIZUZEC9FJZYYCHA
ZRQVFYSECZEIDZTMECCRLBCHBGHWRCGGREWAREWD9TLKZXOKPKAEZCTMNFMUXALVLFPAGQBSYDOAGNZJHGPFOQIAADNAQMAUOSXNH9RGUXUIJ9LR9NZAOB9XRJONKZZHWOZUMPZINXPQXTXHJMWIZUHEC9FJZYYCHA
```

## Collaborators
- Michael Colavita
- Garrett Tanzer
