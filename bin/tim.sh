
awk 'BEGIN   { avg = -1; last=-1; }
     /go w/  {
               if (last!=-1)
               {
                 t=last+100-$4;
                 if (t>0)
                 {
                   if (avg==-1)
                     avg=t;
                   else
                     avg=0.7*avg+0.3*t;
                 }
                 else
                 {
                   if ($4 > 10000)
                     wh=12
                   else
                     wh=10
                   l=0
                   avg=-1;
                 }
               }
               else
               {
                 if ($4 > 10000)
                   wh=12
                 else
                   wh=10
                 l=0
               }
               avgp=""
               if (l==6)
                 avgp=avg
               print $0, t, wh, avgp;
               last=$4;
               l=l+1
             }
    ' ${1:-sfnew.log} | less
