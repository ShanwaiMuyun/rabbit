import httpInstance from '@/utils/http'

const toGoods = (item) => ({
    id: item.id,
    name: item.name,
    desc: item.description,
    price: Number(item.price),
    picture: item.image_url,
    stock: Number(item.stock),
    seller: item.seller_username,
    category: item.category || 'others'
})

const categoryOptions = [
    { id: 'digital', name: '数码电器' },
    { id: 'fresh-food', name: '食品生鲜' },
    { id: 'bags', name: '箱包配件' },
    { id: 'others', name: '其他商品' }
]

const groupGoods = (goods) => {
    const groups = categoryOptions.map((option) => {
        const list = goods.filter((item) => item.category === option.id)

        return {
            id: option.id,
            name: option.name,
            picture: list[0]?.picture || goods[0]?.picture || '',
            goods: list
        }
    })

    return groups.filter((item) => item.goods.length)
}

export async function getCategoryAPI() {
    const res = await httpInstance({
        url: '/products'
    })

    const goods = (res.data || []).map(toGoods)

    return {
        result: [{
            id: 'online',
            name: '在线商品',
            picture: goods[0]?.picture || '',
            children: groupGoods(goods),
            goods
        }]
    }
}
