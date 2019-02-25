# Statistika - Vezbe 10
# asistent: Tamara Milic 
# Zorana Gajic 

# 1. Na osnovu prethodnih istrazivanja prosecna starost devojaka
# prilikom prvog sastanka bila je 14 godina. 
# Sociolozi tvrde da se to promenilo. Sakupljen je uzorak od 
# 15 devojaka i postavljeno im je ovo pitanje. 
# Dobijeni su rezultati: 
x = c(13, 12.5, 13.5, 14.2, 11.5, 12.5, 15, 15.5, 13.5, 13, 16, 15.5, 13.7, 12, 14.5) 
# Ako je alfa=0.05 testom znakova ispitati da li su sociolozi u pravu.

qplus = sum(x>14)
n = length(x)
p.value = pbinom(qplus, n, 1/2)
p.value < 0.05 
# => prihvatamo H0 


# 2. Prevoznik razmatra promenu trase putovanja koriscenjem novog autoputa. 
# Prosecno vreme putovanja na staroj trasi je 4 sata. Prilikom 18 probnih 
# voznji na novoj trasi, dobijeni su rezultati: 
x = c(4.4, 3.9, 5.2, 4.6, 4.3, 3.6, 4.4, 5.5, 3.8, 4.7, 4.1, 3.8, 4.8, 4.5, 5.6, 4.1, 4.5, 4)
qplus = sum(x>4)
n = length(x)
p.value = pbinom(qplus, n, 1/2)
p.value < 0.05
# => prihvatamo H0

# 3. Procenat nepismenih u Americi 1969. je bio 0.7. Smatra se da je u 
# velikim gradovima veci. Izabrano je 20 gradova i dobijeni su podaci: 
x = c(0.75, 1.0, 0.69, 0.8, 0.57, 0.9, 1.5, 0.95, 0.6, 0.5, 0.62, 1.7, 0.53, 1.1, 1.2, 2.0, 0.65, 0.79, 0.61) 
# Za alfa=0.05 Vilkoksonovim testom oznacenih rangova testirati tvrdnju

wilcox.test(x, mu = 0.7, alternative = "greater")
p.value = 0.03314 
p.value < 0.05
# TRUE => upali smo u kriticnu oblast => odbaci H0

# 4. Postoji tvrdnja da je prosecno trajanje telefonskog poziva 3 min. 
# Neki smatraju da je vece. Uzet je uzorak od 10 slucajno izabranih 
# poziva: 
x = c(2.7, 10.5, 3.8, 15.2, 5.7, 3.5, 2.1, 4.0, 3.7) 
# Za alfa = 0.01 Vilkoksonovim testom oznacenih rangova ispitati da li prosecno trajanje vece.

wilcox.test(x, mu = 3, alternative = "greater")
p.value = 0.02734
alfa = 0.01
p.value > alfa
# TRUE => prihvatamo H0

# 5.Proizvodjac sapuna zeli da pokaze kako njegov novi sapun ne isusuje kozu. Uzet je uzorak od 10 zena 
# i svaka je 2 nedelje prala jednu polovinu lica tim sapunom a drugu sapunom drugog proizvodjaca. 
# Na kraju je merena suvoca lica ( na skali od 1 do 10, manji broj znaci vecu suvocu). Dobijeni su 
# rezultati:
x <- c(5, 4.3, 7.3, 2.1 , 9.8, 6.9, 10, 1.5, 8.2, 7.3)
y <- c(6.1, 4.5, 6.0, 2.0, 7.5, 8, 9.2, 1, 8, 6.9)

# Za alfa = 0.05 testom znakova ispitati da li je novi bolji od sapuna drugog proizvodjaca.

alfa = 0.05
d = x-y
n = length(d)
q = sum(d < 0)
p = pbinom(q, n, 1/2)
p > alfa
# TRUE => prihvatamo H0

# 6. Farmaceutska kompanija ima dve metode za proizvodnju odredjenog leka. Smatra se da je metoda 
# A manje uspesna od metode B. Dat je uzorak od 12 pacijenata koji su primili oba tretmana. 
# Vilkoksonovim testom za alfa = 0.05 ispitati H0: miA = miB protiv H1: miA < miB

A = c(1.5, 1.4, 1.4, 1, 1.1, 0.9, 1.3, 1.2, 1.1, 0.9, 0.7, 1.8)
B = c(2, 1.8, 0.7, 1.3, 1.2, 1.5, 1.1, 0.9, 1.5, 1.7, 0.9, 0.9)
wilcox.test(A-B, 0, alternative="less")
p.value = 0.3846
p.value > 0.05
# TRUE => prihvatamo H0
