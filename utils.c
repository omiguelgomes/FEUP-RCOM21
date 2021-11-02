char[5] create_trama(int role, int tramaType)
{
    char[5] return_trama;
    return_trama[0] = FLAG;
    //comands sent by sender, or returns by receiver
    if((role == SENDER && tramaType <= 1) || (role == RECEIVER && tramaType > 1))
    {
        return_trama[1] = 0x03;
    }
    else
    {
        return_trama[1] = 0x01;
    }
    switch(tramaType)
    {
        case(0):
            return_trama[2] = 0x03;
            break;
        case(1):
            return_trama[2] = 0x0B;
            break;
        case(2):
            return_trama[2] = 0x07;
            break;
        case(3):
            break;
        case(4):
            break;
    }
    return_trama[3] = return_trama[1] ^ return_trama[2];
    return_trama[4] = return_trama[0];
    return return_trama;
}
