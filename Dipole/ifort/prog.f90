program main
real (kind=8), dimension(23):: x,y,z,m
real (kind=8), dimension (23):: s
real(kind=8) ,dimension(23,23)::r
integer(kind=4) i,n,l,j,nn,t
real(kind=8) a

write(*,*) "enter t"
read (*,*) t

open(unit=1,file="x5")
open(unit=2,file="y5")
open(unit=3,file="z5")
open(unit=4,file="m5")
do i=1,t
    read(1,*) x(i)
    read(2,*) y(i)
    read(3,*) z(i)
    read(4,*) m(i)
end do

write(*,*) "enter a and n"
read(*,*)  a,nn

do n=0,nn
    do l=0,n
        if (n==0) then
            do i=1,t
                do j=1,t
                    if (i/=j) then
                        r(i,j)=(m(i)*m(j))*((2*(z(i)-z(j))**2-(x(i)-x(j))**2-(y(i)-y(j))**2)/&
                        ((z(i)-z(j))**2+(x(i)-x(j))**2+(y(i)-y(j))**2)**(2.5))
                    endif
                end do
            end do
        else
            do i=1,t
                do j=1,t
                    r(i,j)=(m(i)*m(j))*((2*(z(i)-z(j))**2-(x(i)-x(j)+n*a-0.5*l*a)**2-(y(i)-y(j)+l*1.7320508*0.5*a)**2)/&
                    ((z(i)-z(j))**2+(x(i)-x(j)+n*a-0.5*l*a)**2+(y(i)-y(j)+1.7320508*0.5*l*a)**2)**(2.5))
                end do
            end do
        endif

        s(n)=0

        do i=1,t
            do j=1,t
                if (n==0) then
                    s(n)=s(n)+r(i,j)
                else
                    if(l==0) then
                        s(n)=s(n)+4*r(i,j)
                    else if(l==n) then
                        s(n)=s(n)+4*r(i,j)
                    else
                        s(n)=s(n)+8*r(i,j)
                    endif          
                endif
            end do
        end do

        open(unit=5,file="sum")
        write(5,*) s(n)
    end do
end do
end program main 