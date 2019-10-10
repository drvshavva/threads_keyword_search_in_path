/*Havanur Dervişoğlu 17011604*/
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include<string.h>

#define m_boyut 100
#define NUM_THREADS 100

/*bu programda giriş değerlerini bir structta tuttuk kullanıcı aranacak kelime sayısını belirliyor ve daha sonra keyword ve dizin kullanıcıdan girilmesi bekleniyor.Her giriş yeni bir thread oluşturuyor ve bu arama threadında verilen dizindeki txt uzantılı dosyalarda keywordu arayıp arama numarısı.txt adlı yeni dosyaya kaydediyor*/


//bu structta aranacak kelimeyi,bulunduğu dizini ve kaçıncı arama olduğunu tuttuk
struct arama{
	char keyword[m_boyut];
        int index;
	char dizin[m_boyut];
};
//bu fonksiyon hataların nerelerden kaynaklı olduğunu gösterir
void err_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void * f_arama(void *threadid){
	FILE *fp1;//arama sonucu oluşturulacak olan file ın pointerı
	FILE *fp;//arama olunacak fileın pointerı

	struct arama *tid;
	tid=(struct arama*)threadid;

	DIR *dir;
    	struct dirent *ent;
    	struct stat finfo;

	char dosya[m_boyut];//dosya isimlendirmesi için aşağıda kullanılacak
	char ch;//aşağıda keyword bulmak için dosyadan kullanılcak
	int i=0,k=1;//satır sayısı ve keyword index
	int num=0;//dosyadaki keyword sayısını tutacak

	//arama sayısını bir arttırıyoruz
	tid->index++;

		
	
	//yeni oluşturulacak olan dosyanın isimlendirilmesi
	char i2[m_boyut];
	i2[0]=tid->index+'0';//integerdan sayımızı char e convert etme
	i2[1]='.';
	i2[2]='t';
	i2[3]='x';
	i2[4]='t';
	if((fp1=fopen(i2,"w"))==NULL){
		err_sys("fopen,write");
		exit(0);
	}
	
	//girilen dizini açmak için opendir kullandık ve başarısızlık durumunu kontrol etme
	if ((dir = opendir(tid->dizin)) == NULL)
        	err_sys("opendir");
	
	//threadın çalışma dizini belirtilen dizin oldu,dizin değiştirme işlemidir,eklenmediğinde stat hata veriyor
    	if (chdir(tid->dizin) < 0)
        	err_sys("chdir");

	//readdir fonksiyonuyla dizindeki elemanlar okunuyor ,ve stat fonksiyonu ile dosya bilgilerine ulaşılıyor
	while (errno = 0, (ent = readdir(dir)) != NULL) {
        	if (stat(ent->d_name, &finfo) < 0)
           		 err_sys("stat");

       		 
       		 if(strstr(ent->d_name,".txt")){//dosyanın .txt uzantılı olup olmadığını kontrol ediliyor
			strcpy(dosya,ent->d_name);//açılıp okunacak olan dosyanın adı belirlenir
			if((fp=fopen(dosya,"r"))==NULL){
				err_sys("fopen:read");
				exit(0);
			}
			else{
			 	k=1;//satır saysını ilk satıra alsın diye yeniden txt dosyası açılınca
				num=0;//aynı dizine tekrardan giridiğinde saymaya baştan başlasın
				fprintf(fp1,"<----%s---->\n",dosya);
				while(!feof(fp)){
				   fscanf(fp,"%c",&ch);//keywordu karakter karakter okuyoruz
				   
				   if(tid->keyword[i]==ch){//burada ilk karakterleri keywordle karşılaştırıyoruz dosyadan 			
					    
					   if(i==(strlen(tid->keyword)-1)){//bu kelimenin dosyada bulunduğunu kontrol eder 
					     num++;//keyword sayısını arttırdık
					     i=0;//keywordun karakter indisini sıfıra getirdik
					     fprintf(fp1,"%d.satır\n",k);//hangi satırda olduğunu dosyaya yazdırdık
					    }
					    else i++;//eğer karakter sayısı eşleşmediyse keywordun karakter indisini bir arttırdım bir sonraki karakterleri karşılaştırmak için
			           }
  				   else {
        				i=0;
				        if(ch=='\n'){//eğer karakter new line sa yeni satır demek
					    k++;//satır sayısını arttırıyoruz
				         }
         		           }//bir sonraki karşılaştırma için indisi 0lıyor eğer diğer karakterlerin eşlemesinde benzerlik yoksa baştan alır keyword indisi
			        }
				
			    fprintf(fp1,"%s :%s den  %d tane  var\n",dosya,tid->keyword,num);//yazmak için açtığımız dosyaya aradığımız kelimeden kaç tane olduğunu yazdırdık	   
		                	   	 
		        }
			fclose(fp);//okumak için açtığımız dosyayı kapattık

                }		
       }
    	if (errno)
        	err_sys("readdir");
        fclose(fp1);//yazdığımız dosyayı kapattık
        closedir(dir);//açtığımız dizini kapattık
   	pthread_exit(NULL);
}
int main( void ){
	
	struct arama *d;
	d=(struct arama*)malloc(sizeof(struct arama));

	int num;//girilecek keyword sayısını tutar
	int j;
	int rc;
	
	pthread_t threads[NUM_THREADS];

	
	d->index=0;

	printf("aranacak kelime sayısını giriniz:\n");
	scanf("%d",&num);
        
	//aranacak keyword sayısı kadar thread oluşturduk
	for(j=0;j<num;j++){

	scanf("%s",d->keyword);
	scanf("%s",d->dizin);
	rc=pthread_create(&threads[j],NULL,f_arama,d);
	
		if(rc){
			err_sys("pthread_create");
			return -1;
		}
	}
	
	for(j=0;j<num;j++){
        	pthread_join(threads[j],NULL);
	}
	return 0;
}
	
