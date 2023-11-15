/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Paulo Yamagishi */
/* Laboratório: 8 */
/* Codigo: Criando um pool de threads em Java e contando primos*/

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

//-------------------------------------------------------------------------------
//Classe que define um pool de threads 
class FilaTarefas {
    private final int nThreads;
    private final MyPoolThreads[] threads;
    private final LinkedList<Runnable> queue;
    private boolean shutdown;

    public FilaTarefas(int nThreads) {
        this.shutdown = false;
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();
        threads = new MyPoolThreads[nThreads];
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    public void execute(Runnable r) {
        synchronized(queue) {
            if (this.shutdown) return;
            queue.addLast(r);
            queue.notify();
        }
    }
    
    public void shutdown() {
        synchronized(queue) {
            this.shutdown=true;
            queue.notifyAll();
        }
        for (int i=0; i<nThreads; i++) {
          try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
    }

    private class MyPoolThreads extends Thread {
       public void run() {
         Runnable r;
         while (true) {
           synchronized(queue) {
             while (queue.isEmpty() && (!shutdown)) {
               try { queue.wait(); }
               catch (InterruptedException ignored){}
             }
             if (queue.isEmpty() && shutdown) return;  
             r = (Runnable) queue.removeFirst();
           }
           try { r.run(); }
           catch (RuntimeException e) {}
         } 
       } 
    } 
}
//-------------------------------------------------------------------------------

class Contador {
  //recurso compartilhado
  private long r;
  //construtor
  public Contador() { this.r = 0; }

  //operacao sobre o recurso compartilhado
  public synchronized void inc() { this.r++; }

  //operacao sobre o recurso compartilhado
  public synchronized long get() { return this.r; }
}

//--PASSO 1: cria uma classe que implementa a interface Runnable 
class QtdPrimos implements Runnable {
   private long numero;
   private Contador qtd_primos;
   public QtdPrimos(long numero, Contador qtd_primos) { 
      this.numero = numero; 
      this.qtd_primos = qtd_primos; 
   }

   //--metodo executado pela thread
   public void run() {
      if (ehPrimo(numero)) {
        qtd_primos.inc();
      }
   }

   private boolean ehPrimo(long numero) {
      int i;
      if (numero<=1) return false;
      if (numero==2) return true;
      if (numero%2==0) return false;
      for (i=3; i<Math.sqrt(numero)+1; i+=2)
      if(numero%i ==0) return false;
      return true;
   }
}

class PrimoSequencial{
  public static long n_Primos(long n) {
    int qtd_primos = 0;
    for (long i = 0; i < n; i++) {
      if (ehPrimo(i)) {
        qtd_primos++;
      }
    }
    return qtd_primos;
  }

  private static boolean ehPrimo(long numero) {
    int i;
    if (numero<=1) return false;
    if (numero==2) return true;
    if (numero%2==0) return false;
    for (i=3; i<Math.sqrt(numero)+1; i+=2)
    if(numero%i ==0) return false;
    return true;
  }
}

//Classe da aplicação (método main)
class Primos {
    private static final int NTHREADS = 10;
    private static final long n = 200000;

    public static void main (String[] args) {
      //--PASSO 2: cria o pool de threads
      FilaTarefas pool = new FilaTarefas(NTHREADS); 
      
      // Criando um contador para cada thread para possibilitar maior concorrência
      List<Contador> qtd_de_primos = new ArrayList<Contador>();
      for (int i = 0; i < NTHREADS; i++) {
        qtd_de_primos.add(new Contador());
      }

      //--PASSO 3: dispara a execução dos objetos runnable usando o pool de threads
      for (long i = 0; i < n; i++) {
        Runnable r = new QtdPrimos((i+1), qtd_de_primos.get((int)i%NTHREADS));
        pool.execute(r);
      }
      
      //--PASSO 4: esperar pelo termino das threads
      pool.shutdown();
      System.out.println("Terminou");

      //Somando os contadores
      int sum = 0;
      for (int i = 0; i < NTHREADS; i++) {
        sum += qtd_de_primos.get(i).get();
      }

      //Verificando se o algoritmo está correto
      if (sum == PrimoSequencial.n_Primos(n)) {
        System.out.println("Algortimo correto");
        System.out.println("Quantidade de primos: " + sum);
      }
      else {
        System.out.println("Algortimo incorreto");
      }
    }
}
