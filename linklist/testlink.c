
struct CON_list {
    CONNECTION_INFO_ST connection_info;
    struct CON_list *next;
    struct CON_list *previous;
};
void insert_con_node(struct CON_list *data)
{
    /*empty con list*/
    if (con_head_node == NULL) {
        data->next = NULL;
        data->previous = NULL;
        con_head_node = con_last_node = con_current_node = data;
    } else {
        struct CON_list *tmp = con_current_node, *tmp1 = NULL;
        if (data->connection_info.CID > tmp->connection_info.CID) {
            do {
                if (tmp->next == NULL) {
                    break;
                }
                tmp = tmp->next;
            } while (data->connection_info.CID > tmp->connection_info.CID);

            if ((tmp == con_last_node) && (data->connection_info.CID > con_last_node->connection_info.CID)) {
                con_last_node->next = data;
                data->previous = con_last_node;
                data->next = NULL;
                con_last_node = con_current_node = data;
            } else {
                tmp1 = tmp->previous;
                data->previous = tmp1;
                data->next = tmp;
                tmp1->next = tmp->previous = data;
                con_current_node = data;
            }
        } else if (data->connection_info.CID < tmp->connection_info.CID) {
            do {
                if (tmp->previous == NULL) {
                    break;
                }
                tmp = tmp->previous;
            } while (data->connection_info.CID < tmp->connection_info.CID);

            if ((tmp == con_head_node) && (data->connection_info.CID < con_head_node->connection_info.CID)) {
                con_head_node->previous = data;
                data->next = con_head_node;
                data->previous = NULL;
                con_head_node = con_current_node = data;
            } else {
                tmp1 = tmp->next;
                data->next = tmp1;
                data->previous = tmp;
                tmp1->previous = tmp->next = data;
                con_current_node = data;
            }
        }
    }
}

struct CON_list *find_con_node(uint32_t CID)
{
    struct CON_list *tmp = con_current_node;
    if (CID > tmp->connection_info.CID) {
        do {
            if (tmp->next == NULL) {
                goto MISMATCH_CID;
            }
            tmp = tmp->next;
        } while (CID != tmp->connection_info.CID);
    } else if (CID < tmp->connection_info.CID) {
        do {
            if (tmp->previous == NULL) {
                goto MISMATCH_CID;
            }
            tmp = tmp->previous;
        } while (CID != tmp->connection_info.CID);
    }
    con_current_node = tmp;
    return tmp;
 MISMATCH_CID:
    return NULL;
}

struct CON_list *remove_and_relink_node(struct CON_list *node)
{
    struct CON_list *tmp1, *tmp2;
    tmp1 = node->next;
    tmp2 = node->previous;
    tmp1->previous = tmp2;
    tmp2->next = tmp1;
    return tmp1;
}

struct CON_list *remove_con_node(uint32_t CID)
{
    if (CID == con_current_node->connection_info.CID) {
        struct CON_list *tmp = con_current_node;
        if ((tmp->next == NULL) && (tmp->previous == NULL)) {
            con_current_node = con_head_node = con_last_node = NULL;
        } else if (tmp == con_head_node) {
            con_head_node = con_current_node = con_head_node->next;
            con_head_node->previous = NULL;
        } else if (tmp == con_last_node) {
            con_last_node = con_current_node = con_last_node->previous;
            con_last_node->next = NULL;
        } else {
            con_current_node = remove_and_relink_node(con_current_node);
        }
        return tmp;
    } else if (CID > con_current_node->connection_info.CID) {
        struct CON_list *tmp = con_current_node;
        do {
            if (tmp->next == NULL) {
                goto MISMATCH_CID;
            }
            tmp = tmp->next;
        } while (CID != tmp->connection_info.CID);

        if (tmp == con_last_node) {
            con_last_node = con_current_node = con_last_node->previous;
            con_last_node->next = NULL;
        } else {
            con_current_node = remove_and_relink_node(tmp);
        }
        return tmp;
    } else {
        struct CON_list *tmp = con_current_node;
        do {
            if (tmp->previous == NULL) {
                goto MISMATCH_CID;
            }
            tmp = tmp->previous;
        } while (CID != tmp->connection_info.CID);

        if (tmp == con_head_node) {
            con_head_node = con_current_node = con_head_node->next;
            con_last_node->previous = NULL;
        } else {
            con_current_node = remove_and_relink_node(tmp);
        }
        return tmp;
    }
 MISMATCH_CID:
    return NULL;
}

