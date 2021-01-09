program main
real (kind=8), dimension(231):: x,S
integer(kind=4) i,j

open(unit=1,file="sum")
do i=1,231
    read(1,*) x(i)
end do

do i=1,231
    S(i)=0
    do j=1,i
        S(i)=S(i)+x(j)
    end do
    open(unit=2,file="conv")
    write(2,*) S(i), S(i)/(4*3.14159)
end do
end program main
