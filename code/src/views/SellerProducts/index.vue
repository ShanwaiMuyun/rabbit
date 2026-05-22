<script setup>
import { computed, onMounted, ref } from 'vue'
import { ElMessage } from 'element-plus'
import { deleteProductAPI, getProductsAPI, publishProductAPI, updateProductAPI } from '@/apis/product'
import { useUserStore } from '@/stores/userStore'

const userStore = useUserStore()
const products = ref([])
const loading = ref(false)
const dialogVisible = ref(false)
const formRef = ref(null)
const editingId = ref(null)

const seller = computed(() => userStore.userInfo?.username || '')
const canManage = computed(() => userStore.userInfo?.role === 'seller')

const categoryOptions = [
    { label: '数码电器', value: 'digital' },
    { label: '食品生鲜', value: 'fresh-food' },
    { label: '箱包配件', value: 'bags' },
    { label: '其他商品', value: 'others' }
]

const getCategoryName = (value) => {
    return categoryOptions.find((item) => item.value === value)?.label || '其他商品'
}

const form = ref({
    name: '',
    description: '',
    category: 'digital',
    imageUrl: '',
    price: 1,
    stock: 1
})

const rules = {
    name: [{ required: true, message: '请输入商品名称', trigger: 'blur' }],
    category: [{ required: true, message: '请选择商品类别', trigger: 'change' }],
    imageUrl: [{ required: true, message: '请输入商品图片 URL', trigger: 'blur' }],
    price: [{ required: true, message: '请输入商品价格', trigger: 'blur' }],
    stock: [{ required: true, message: '请输入库存数量', trigger: 'blur' }]
}

const statusMap = {
    pending: { text: '待审核', type: 'warning' },
    approved: { text: '已通过', type: 'success' },
    rejected: { text: '已拒绝', type: 'danger' }
}

const resetForm = () => {
    editingId.value = null
    form.value = {
        name: '',
        description: '',
        category: 'digital',
        imageUrl: '',
        price: 1,
        stock: 1
    }
}

const loadProducts = async () => {
    if (!seller.value) {
        return
    }

    loading.value = true
    try {
        const res = await getProductsAPI({ approvedOnly: 0 })
        products.value = (res.data || []).filter((item) => item.seller_username === seller.value)
    } finally {
        loading.value = false
    }
}

const openCreate = () => {
    resetForm()
    dialogVisible.value = true
}

const openEdit = (row) => {
    editingId.value = row.id
    form.value = {
        name: row.name,
        description: row.description,
        category: row.category || 'others',
        imageUrl: row.image_url,
        price: Number(row.price),
        stock: Number(row.stock)
    }
    dialogVisible.value = true
}

const submitProduct = () => {
    formRef.value.validate(async (valid) => {
        if (!valid) {
            return
        }

        const payload = {
            seller: seller.value,
            ...form.value
        }

        if (editingId.value) {
            await updateProductAPI({
                productId: editingId.value,
                ...payload
            })
            ElMessage.success('商品已修改，请等待管理员重新审核')
        } else {
            await publishProductAPI(payload)
            ElMessage.success('商品已发布，请等待管理员审核')
        }

        dialogVisible.value = false
        loadProducts()
    })
}

const deleteProduct = async (row) => {
    await deleteProductAPI({
        productId: row.id,
        seller: seller.value
    })
    ElMessage.success('商品已删除')
    loadProducts()
}

onMounted(() => loadProducts())
</script>

<template>
    <div class="seller-page">
        <div class="container">
            <div class="page-head">
                <div>
                    <h2>商品管理</h2>
                    <p>发布和维护商品信息，商品通过审核后会展示给买家</p>
                </div>
                <el-button type="primary" :disabled="!canManage" @click="openCreate">发布商品</el-button>
            </div>

            <el-alert
                v-if="!canManage"
                title="当前账号不是卖家。请注册卖家账号并通过管理员审核后使用商品管理。"
                type="warning"
                show-icon
                :closable="false"
            />

            <section v-else class="panel">
                <el-table v-loading="loading" :data="products" border>
                    <el-table-column label="图片" width="100">
                        <template #default="{ row }">
                            <img class="product-img" :src="row.image_url" alt="" />
                        </template>
                    </el-table-column>
                    <el-table-column prop="name" label="商品名称" min-width="160" />
                    <el-table-column label="类别" width="110">
                        <template #default="{ row }">
                            {{ getCategoryName(row.category) }}
                        </template>
                    </el-table-column>
                    <el-table-column prop="description" label="描述" min-width="220" />
                    <el-table-column prop="price" label="价格" width="100" />
                    <el-table-column prop="stock" label="库存" width="90" />
                    <el-table-column label="状态" width="100">
                        <template #default="{ row }">
                            <el-tag :type="statusMap[row.status]?.type">
                                {{ statusMap[row.status]?.text || row.status }}
                            </el-tag>
                        </template>
                    </el-table-column>
                    <el-table-column label="操作" width="180">
                        <template #default="{ row }">
                            <el-button size="small" @click="openEdit(row)">修改</el-button>
                            <el-popconfirm title="确认删除该商品吗？" @confirm="deleteProduct(row)">
                                <template #reference>
                                    <el-button size="small" type="danger">删除</el-button>
                                </template>
                            </el-popconfirm>
                        </template>
                    </el-table-column>
                </el-table>
                <el-empty v-if="!loading && products.length === 0" description="暂无商品，先发布一个吧" />
            </section>
        </div>

        <el-dialog v-model="dialogVisible" :title="editingId ? '修改商品' : '发布商品'" width="520px">
            <el-form ref="formRef" :model="form" :rules="rules" label-width="96px">
                <el-form-item prop="name" label="商品名称">
                    <el-input v-model="form.name" />
                </el-form-item>
                <el-form-item prop="description" label="商品描述">
                    <el-input v-model="form.description" type="textarea" :rows="3" />
                </el-form-item>
                <el-form-item prop="category" label="商品类别">
                    <el-select v-model="form.category" placeholder="请选择商品类别" style="width: 100%">
                        <el-option
                            v-for="item in categoryOptions"
                            :key="item.value"
                            :label="item.label"
                            :value="item.value"
                        />
                    </el-select>
                </el-form-item>
                <el-form-item prop="imageUrl" label="图片 URL">
                    <el-input v-model="form.imageUrl" />
                </el-form-item>
                <el-form-item prop="price" label="价格">
                    <el-input-number v-model="form.price" :min="0.01" :precision="2" />
                </el-form-item>
                <el-form-item prop="stock" label="库存">
                    <el-input-number v-model="form.stock" :min="0" :step="1" />
                </el-form-item>
            </el-form>
            <template #footer>
                <el-button @click="dialogVisible = false">取消</el-button>
                <el-button type="primary" @click="submitProduct">提交</el-button>
            </template>
        </el-dialog>
    </div>
</template>

<style scoped lang="scss">
.seller-page {
    padding: 24px 0 60px;
    min-height: 640px;
    background: #f5f5f5;
}

.page-head {
    height: 96px;
    display: flex;
    align-items: center;
    justify-content: space-between;
    background: #fff;
    padding: 0 24px;
    margin-bottom: 20px;
    border-radius: 6px;

    h2 {
        font-size: 24px;
        font-weight: 500;
    }

    p {
        color: #777;
        margin-top: 8px;
    }
}

.panel {
    background: #fff;
    padding: 24px;
    border-radius: 6px;
}

.product-img {
    width: 64px;
    height: 64px;
    object-fit: cover;
    border-radius: 4px;
}
</style>
